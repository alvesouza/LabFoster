#include<iostream>
#include<string>
#include<vector>
#include<fstream>

struct transicao{
    int src,dest;
    std::string regExp;
};
struct estado{
    bool valida;//se true o estado valida a cadeia
    std::string number;//numero do estado
    std::vector<transicao> transicoes;
};

class Graph{
    public:
        std::vector<estado> estados;
        std::string regularExp;
        std::vector<std::vector<int>> fecho;
        /*Graph(std::string regExp,int questNumber)
        {
            estados.resize(0);
            regularExp = "";
            if(regExp != "")
                createGraph(regExp, questNumber);
        }*/
        
        //cria o automato(grafo), regExp é a expressão regular e o questNumber é a questão(automato testado, será assim baseado nisso que o arquivo será nomeado)
        void createGraph(std::string regExp,int questNumber)
        {   
            //zera o automato, caso a instancia seja reutilizada
            estados.resize(0);
            
            //Avisa terminal, que começou a rodar
            std::cout << "Comecando o grafo para " << regExp << std::endl;

            //cria e abre o arquivo txt, precisei fazer assim e não dentro da função em sí, pois quero saber todos os processos de mudança
            std::string fileName = "expressaoRegular";
            questionNumber = questNumber;
            fileName.append(std::to_string(questionNumber));
            fileName.append(".txt");
            myFile.open(fileName);

            regularExp = regExp;//regularExp é constante, caso eu queira saber o equivalente do automato
            

            //cria o automato 0 -> 1
            transicao trans;
            estado aux;
            trans.src = 0;
            trans.dest = 1;
            trans.regExp = regExp;
            aux.transicoes.push_back(trans);//estado 0
            aux.valida = false;
            aux.number = std::to_string(0);
            estados.push_back(aux);//adciona estado 0
            aux.transicoes.resize(0);
            aux.valida = true;
            aux.number = std::to_string(1);;
            estados.push_back(aux);//adciona estado 1
            
            //printa o automato inicial
            printGraphInTXT();
            
            //altera a transição 0 -> 1(primeira transicao de 0)
            changeTrans(0,0);

            //fecha o arquivo
            myFile.close();

            //Avisa terminal, que terminou de rodar
            std::cout << "Terminou o grafo para " << regularExp << std::endl;
        }

        //escreve arquivo texto para o graphviz
        void printGraphInTXT()
        {
            int numberStates = estados.size();
            myFile <<"Etapa "<< etapa++ << ":\n\n";
            myFile << "digraph finite_state_machine {\n\trankdir=LR;\n\tsize=\"8,5\"\n\t";
            myFile << "node [shape = doublecircle];";
            for(int i = 0;i < numberStates;i++)
            {
                if(estados[i].valida)
                    myFile << " " << estados[i].number;
            }
            myFile << "\n\tnode [shape = circle];\n\t";
            for(int i = 0;i < numberStates;i++)
            {
                for(int j = 0, m = estados[i].transicoes.size(); j<m; j++)
                    {
                        myFile << estados[i].number <<" -> " << estados[estados[i].transicoes[j].dest].number
                             << " [ label = \"" << estados[i].transicoes[j].regExp <<"\" ];\n";
                        if(!(i+1 == numberStates)||!(j+1 == m))
                            myFile <<"\t";
                    }
            }
            myFile << "}\n\n//////////////////////////////////////////////\n\n";
        }

        //encontra os possiveis estados e verifica se a cadeia é valida
        bool cadeiaValida(std::string cadeia)
        {
            bool valida = false;
            std::vector<int> possiveisEstados;

            std::cout << "Possiveis estados:\n";

            passaPorAutomato(possiveisEstados, cadeia, 0, 0);//passa pela cadeia e automato

            //imprimi os estados possiveis e verifica se é valida
            for(int i = 0,n = possiveisEstados.size();i<n;i++)
            {
                std::cout << "estado: "<< estados[possiveisEstados[i]].number;
                
                //escreve se esse estado valida ou não a cadeia
                if(estados[possiveisEstados[i]].valida)
                {
                    std::cout << " EH VALIDA" << std::endl;
                    valida = true;
                }
                else std::cout << " NAO EH VALIDA" << std::endl;
            }

            return valida;

        }
        
        // método genérico para remover as transições epsilon
        void removeEpsilonTrans (int questNumber=0){
            // gera arquivo de saída
            std::string fileName = "remocaoEpsilon";
            fileName.append(std::to_string(questNumber));
            fileName.append(".txt");
            myFile.open(fileName); 
            etapa=0;
            computaFechos();
            printGraphInTXT();
            addArcoReduntanteI();
            printGraphInTXT();
            addArcoReduntanteII();
            printGraphInTXT();
            setValidState();
            printGraphInTXT();
            removeEpsilon(); 
            printGraphInTXT();

            //fecha o arquivo
            myFile.close();

        }

        // Métodos para as remoções das E-trnasições (pode resultar em múltiplos estados finais)
        // I-computar o fecho de cada estado
        void computaFechos(){
            std::cout << "I - Computar o transicoes epsilon de cada estado" << std::endl;
            fecho.resize(estados.size());
            // primeiro passo: todo estado está no seu próprio fecho
            for (int i=0;i<fecho.size();i++){
                fecho[i].push_back(i);
            }
            for (int i=0;i<fecho.size();i++){   // para cada estado
                for (int j=0;j<estados[i].transicoes.size();j++){   // para cada transição de um estado
                    if (estados[i].transicoes[j].regExp=="&"){
                        int dest = estados[i].transicoes[j].dest;
                        fecho[i].insert(fecho[i].end(),fecho[dest].begin(),fecho[dest].end());
                    }
                }
            }

            for (int i=0;i<fecho.size();i++){   // percorre cada vetor de fecho e subsitui cada elemento diferente pelo seu fecho
                std::vector<int> aux;
                for (int j=0;j<fecho[i].size();j++){
                    if (fecho[i][j]!=i){
                        int outro = fecho[i][j];
                        aux.insert(aux.end(),fecho[outro].begin(),fecho[outro].end());
                    }
                    else aux.push_back(i);
                }
                fecho[i]=aux;
                aux.clear();
            }
            // só printa pra ver se tá ok
            for (int i=0;i<fecho.size();i++){
                std::cout << "fecho[" << i << "] = ";
                for (int j=0;j<fecho[i].size();j++){
                    std::cout << fecho[i][j] << " ";
                }
                std::cout << std::endl;
            }
        }

        // II- Se Y pertence ao fecho de X, todo arco A -> X gera um arco de A -> Y 
        // basicamente vamos adicionar arcos redundantes
        void addArcoReduntanteI (){
            // percorrer o autômato:            
            for (int i=0;i<estados.size();i++){   // para cada estado
                int counter=0; // conta quantos arcos redundantes por estado foram adicionados => evita loop infinito
                for (int j=0;j<estados[i].transicoes.size()-counter;j++){   // para cada transição de um estado
                    int src = estados[i].transicoes[j].src;
                    int dest = estados[i].transicoes[j].dest;
                    std::string exp = estados[i].transicoes[j].regExp;
                    for (int k=0;k<fecho[dest].size();k++){
                        // adiciona transição redundante
                        if (fecho[dest][k]!=dest){
                            struct transicao trans;
                            trans.src=src; trans.dest=fecho[dest][k]; trans.regExp=exp;
                            if (!existsArc(trans) && exp!="&"){ //se ainda não existe o arco, add
                                estados[i].transicoes.push_back(trans);
                                counter++;
                            }
                        }
                    }
                }
            }
        }
    
        // III- Se Y pertence ao fecho de X, todo arco Y -> A gera um arco de X -> A 
        // basicamente vamos adicionar arcoes redundantes para outro caso
        void addArcoReduntanteII (){
            // percorrer o autômato:            
            for (int i=0;i<estados.size();i++){   // para cada estado
                for (int k=0;k<fecho[i].size();k++){
                    for (int j=0;j<estados[fecho[i][k]].transicoes.size();j++){
                        int dest = estados[fecho[i][k]].transicoes[j].dest;
                        std::string exp = estados[fecho[i][k]].transicoes[j].regExp;
                        // adiciona o arco
                        struct transicao trans;
                        trans.src = i;
                        trans.dest = dest;
                        trans.regExp = exp;
                        if (!existsArc(trans) && exp!="&")
                            estados[i].transicoes.push_back(trans);
                    }
                }
            }
        }

        // IV- Se Y pertence ao fecho de X e Y for final (válido), então X também será válido (porque consegue-se ir X->Y com transição epsilon)
        // basicamente consertar a validade dos estados (nós)
        void setValidState (){
            // percorrer o autômato:            
            for (int i=0;i<estados.size();i++){   // para cada estado
                for (int k=0;k<fecho[i].size();k++){
                    if (estados[fecho[i][k]].valida)
                        estados[i].valida=true;
                }
            }
        }

        void removeEpsilon(){
            // percorrer o autômato:            
            for (int i=0;i<estados.size();i++){   // para cada estado
                for (int j=0;j<estados[i].transicoes.size();j++){   // para cada transição de um estado   
                    int dest = estados[i].transicoes[j].dest;
                    if (estados[i].transicoes[j].regExp=="&"){
                        estados[i].transicoes.erase(estados[i].transicoes.begin()+j);
                    }
                }
            }
        }


        //Retorna a expressão regular equivalente e retorna à etapa 0 da questão 01
        std::string geraRegExp()
        {
            std::string regularExpression = "";
            std::string fileName = "geracaodeExpressaoRegular";
            estado aux;
            int n = 0;
            etapa = 0;
            bool changed = true;
            fileName.append(std::to_string(questionNumber));
            fileName.append(".txt");
            myFile.open(fileName);
            aux.valida = false;
            printGraphInTXT();
            aux.number = "Begin";
            aux.transicoes.resize(1);
            aux.transicoes[0] = {0,1,"&"};
            estados.insert(estados.begin()+0,aux);
            n = estados.size();
            for(int i = 1; i<n; i++)
                for(int j = 0, m = estados[i].transicoes.size(); j < m; j++)
                {
                    estados[i].transicoes[j].dest++;
                    estados[i].transicoes[j].src++;
                }

            printGraphInTXT();

            estados[2].transicoes.push_back({2,n,"&"});
            aux.valida = true;
            aux.number = "End";
            aux.transicoes.resize(0);
            estados.push_back(aux);
            printGraphInTXT();



            while(changed)
            {
                changed = passaPorAutomatoRegExp(0);
            }

            //printGraphInTXT();
            myFile.close();
            return estados[0].transicoes[0].regExp;
        }

    private:
        std::ofstream myFile;
        std::string fileName;
        int questionNumber;
        int etapa;

        void eliminaEstado(int posEstado)
        {
            for(int i = 0,n = estados.size();i<n;i++)
                for(int j = 0, m = estados[i].transicoes.size();j < m; j++)
                    if(estados[i].transicoes[j].dest > posEstado)
                    {
                        estados[i].transicoes[j].dest--;
                        estados[i].transicoes[j].src--;
                    }
            estados.erase(estados.begin()+posEstado);
                
        }

        //passa pelo automato e a cadeia, adcionando os valores de possiveiEstados
        void passaPorAutomato(std::vector<int> &possiveisEstados, std::string &cadeia, int estadoAtual, int posCadeia)
        {
            //caso a cadeia já tenha acabado
            if(posCadeia == cadeia.size())
            {
                possiveisEstados.push_back(estadoAtual);
            }

            //verifica as transições para passar pela cadeia
            for(int i = 0,n = estados[estadoAtual].transicoes.size(); i < n; i++)
            {
                if(estados[estadoAtual].transicoes[i].regExp == "&")//passará sem, se mover na cadeia
                    passaPorAutomato(possiveisEstados, cadeia, estados[estadoAtual].transicoes[i].dest, posCadeia);
                else if(posCadeia < cadeia.size()&&estados[estadoAtual].transicoes[i].regExp[0] ==  cadeia[posCadeia])// passa pela cadeia
                    passaPorAutomato(possiveisEstados, cadeia, estados[estadoAtual].transicoes[i].dest, posCadeia+1);
                
            }
        }

        //irá mudar a transição trans da src(estado) e trans(numero da transição deste estado)
        bool changeTrans(int src, int trans)
        {
            bool changed = false;
            if(estados[src].transicoes[trans].regExp.size()<2)return false;

            changed = verifica2(src, trans);
            if(!changed)
            {
                changed = verifica3(src, trans);
                if(!changed){
                    changed = verifica4(src, trans);
                    if(!changed){
                        changed = verifica5(src, trans);
                        if(!changed)
                            changed = verifica6(src, trans);
                    }    
                }
            }
            //if(changed)printGraphInTXT();
            return changed;
        }

        //separa as transições com '+' 
        bool verifica2(int src, int trans)
        {
            int parenteseAesq = 0,i = 0,n = estados[src].transicoes[trans].regExp.size();//n eh o tamanho da string da expressão regular
            bool changed = false;
            transicao aux;
            std::string splited1,splited2;

            //procura o '+' fora de parenteses para poder dividir as transições (a+b)+ab+c => (a+b) ; ab+c
            while((!changed)&&i<n){
                
                if(estados[src].transicoes[trans].regExp[i]=='(')parenteseAesq++;
                else if(estados[src].transicoes[trans].regExp[i]==')')parenteseAesq--;
                //se for aceito significa que um + fora de parenteses foi encontrado
                else if(estados[src].transicoes[trans].regExp[i]=='+'&&parenteseAesq==0)
                {
                    changed = true;//permite sair do loop
                    splited1 = estados[src].transicoes[trans].regExp.substr(0,i);//pega o i primeiro termos
                    splited2 = estados[src].transicoes[trans].regExp.substr(i+1);//pega a partir do indice i+1(depois de '+')
                    estados[src].transicoes[trans].regExp = splited1;//altera a transição original
                    aux.src = src;//se criará uma outra transição
                    aux.dest = estados[src].transicoes[trans].dest;//o destino e o src serão os mesmos
                    aux.regExp = splited2;
                    estados[src].transicoes.push_back(aux);//adciona a nova as transições do src
                    n = estados[src].transicoes.size()-1;
                    
                    //irá mudar as novas transições se for necessario
                    printGraphInTXT();
                    changeTrans(src,trans);
                    changeTrans(src,n);

                }
                i++;
            }
            return changed;

        }

        //separa a primeira concatenação fora de parenteses encontrada
        //regExp for algo do tipo a(ab+c), ele já não vai entrar por causa dos if's do 'changeTrans'
        //logo ele sempre estara entre parentese ou concatenado
        /*Ex:
            a(ab+c) => a;(ab+c)
            (ab+c)*b => (ab+c)*;b
            a*(ab+c) => a*;(ab+c)
        */
        bool verifica3(int src, int trans)
        {
            int parenteseAesq = 0,n = estados.size(),m = estados[src].transicoes[trans].regExp.size()-1;
            bool changed = false;
            estado newEstado;
            transicao aux;
            std::string splited1,splited2;
            for(int i = 0;(i<m)&&(!changed);i++)//o m garante que não vai dividir algo como (ab+cd)=>(ab+cd);""
            {
                if(estados[src].transicoes[trans].regExp[i]=='(')parenteseAesq++;
                else if(estados[src].transicoes[trans].regExp[i]==')')parenteseAesq--;
                if(parenteseAesq == 0){//quer dizer que fechou parenteses
                    if(estados[src].transicoes[trans].regExp[i+1] == '*')//verifica se é albo como (ab+c)*b
                    {
                        i++;
                        if(i<m)changed = true;//verifica se (ab+c)*b ou (ab+c)*
                    }
                    if(changed||estados[src].transicoes[trans].regExp[i] != '*')
                    {
                        changed = true;
                        splited1 = estados[src].transicoes[trans].regExp.substr(0,i+1);//pega os i+1 primeiros caracteres
                        splited2 = estados[src].transicoes[trans].regExp.substr(i+1);
                        
                        //cria nova trasição
                        aux.regExp = splited2;
                        aux.src = n;//a partir do estado ah ser criado
                        aux.dest = estados[src].transicoes[trans].dest;

                        //altera a nova transição
                        estados[src].transicoes[trans].regExp = splited1;
                        estados[src].transicoes[trans].dest = n;
                        
                        //adciona o a transição ao novo estado
                        newEstado.transicoes.push_back(aux);
                        newEstado.valida = false;
                        newEstado.number = std::to_string(n);

                        //adciona o novo estado
                        estados.push_back(newEstado);
                        
                        //muda as novas transições
                        printGraphInTXT();
                        changeTrans(src,trans);
                        changeTrans(n,0);
                    }
                }
            }
            return changed;  
        }

        //altera o (a+b)*
        bool verifica4(int src, int trans)
        {
            int parenteseAesq = 0,n = estados.size(),m = estados[src].transicoes[trans].regExp.size()-1;
            bool changed = false,ficaloop = true;
            estado newEstado;
            transicao aux;
            std::string splited1;
            for(int i = 0;(i<m)&&ficaloop;i++)
            {
                if(estados[src].transicoes[trans].regExp[i]=='(')parenteseAesq++;
                else if(estados[src].transicoes[trans].regExp[i]==')')parenteseAesq--;
                if(parenteseAesq == 0)
                {
                    ficaloop = false;
                    if(estados[src].transicoes[trans].regExp[++i] == '*'&&i==m){
                        changed = true;
                        splited1 = estados[src].transicoes[trans].regExp.substr(0,i);
                        
                        //novo->dest
                        aux.src = n;
                        aux.dest = estados[src].transicoes[trans].dest;
                        aux.regExp = "&";
                        newEstado.valida = false;
                        newEstado.transicoes.push_back(aux);
                        
                        //src->novo
                        estados[src].transicoes[trans].dest = n;
                        aux.dest = estados[src].transicoes[trans].dest;
                        estados[src].transicoes[trans].regExp = "&";

                        //novo->novo
                        aux.src = n;
                        aux.dest = n;
                        aux.regExp = splited1;
                        newEstado.transicoes.push_back(aux);
                        newEstado.number = std::to_string(n);

                        //cria novo estado
                        estados.push_back(newEstado);
                        
                        printGraphInTXT();

                        //somente o novo->novo sofrerá alteração
                        changeTrans(n,1);
                    }
                }
            }
            return changed;
        
        }

        //completo nao testado
        //verifica se tem parenteses como (a+b)=>a+b ; (a+b)c => (a+b)c
        bool verifica5(int src, int trans)
        {
            int parenteseAesq = 0,m = estados[src].transicoes[trans].regExp.size();
            bool changed = false,ficaloop = true;
            for(int i = 0;(i<m)&&ficaloop;i++)
            {
                if(estados[src].transicoes[trans].regExp[i]=='(')parenteseAesq++;
                else if(estados[src].transicoes[trans].regExp[i]==')')parenteseAesq--;
                if(parenteseAesq == 0)//assim que o parenteses for zero,ou seja todos fecharem, ele vai sair do loop
                {
                    ficaloop = false;
                    if(i==m-1)//caso seja no fim da expressão,significa que é (a+b) e não (a+b)c 
                    {
                        changed = true;
                        
                        //tira '(' e o ')' externos;(a+b)=>a+b
                        estados[src].transicoes[trans].regExp = estados[src].transicoes[trans].regExp.substr(1,m-2);
                        
                        //irá mudar a transição
                        printGraphInTXT();
                        changeTrans(src,trans);
                    }
                }
            }
            return changed;
        
        }

        //caso a expressão regular da transição possua mais de 1 caractere, ele sofrerá mudança
        bool verifica6(int src, int trans)
        {   
            bool changed = false;
            if(estados[src].transicoes[trans].regExp.size()>1)
            {
                changed = changeTrans(src,trans);
            }

            return changed;
        }
        bool passaPorAutomatoRegExp(int src)
        {
            bool changed = true;
            if(src >= estados.size() || estados[src].transicoes.size() == 0)return false;

            changed = padrao01(src);
            changed = changed||padrao02(src);
            

            return changed;
        }
        bool padrao01(int src)
        {
            std::vector<std::string> trans;
            transicao auxTrans;
            //std::string plus = "+";
            bool changed = false;
            int n = estados[src].transicoes.size(),transicoes_finais;
            trans.resize(estados.size());
            for(int i = 0;i < n; i++)
            {
                if(trans[estados[src].transicoes[i].dest].size() > 0)trans[estados[src].transicoes[i].dest].append("+" + estados[src].transicoes[i].regExp);
                else trans[estados[src].transicoes[i].dest].append(estados[src].transicoes[i].regExp);
            }
            
            estados[src].transicoes.resize(0);

            for(int i = 0,m = trans.size();i < m; i++)
            {
                if(trans[i].size() != 0)
                {
                    auxTrans.src = src;
                    auxTrans.dest = i;
                    auxTrans.regExp = trans[i];
                    estados[src].transicoes.push_back(auxTrans);
                }
            }

            changed = (n != estados[src].transicoes.size());

            if(changed)printGraphInTXT();

            return changed;
        }
        void fechaRegularExpression(std::string &regExp)
        {
            int numeroDeMais = 0, parenteseEsq = 0;
            for(int i = 0,n = regExp.size();i < n; i++)
            {
                if(regExp[i] == '(')parenteseEsq++;
                else if(regExp[i] == ')')parenteseEsq--;
                else if(regExp[i] == '+')
                {
                    if(parenteseEsq == 0)
                    {
                        regExp.insert(0, "(");
                        regExp.append(")");
                        i = n;
                    }
                }

            }

        }
        bool padrao02(int src)
        {
            bool changed = false;
            bool changedInFunction = false;
            int apontamPara = 0;
            int estadoApontaPara;
            int estadoParaEliminar = 0;
            int parentesesEsq = 0;
            std::string regExp01,regExp02,regExp03;
            for(int i = 0, n = estados[src].transicoes.size();i < n && !changed; i++)
            {
                apontamPara = 0;
                if(estados[estados[src].transicoes[i].dest].transicoes.size() < 3)
                {
                    for(int nestado = 0,numEstados = estados.size();nestado < numEstados&&apontamPara<2;nestado++)
                    {
                        for(int ntrans = 0,numTrans = estados[nestado].transicoes.size();ntrans<numTrans&&apontamPara<2;ntrans++)
                            if(estados[nestado].transicoes[ntrans].dest == estados[src].transicoes[i].dest)apontamPara++;

                        if(nestado+1 == estados[src].transicoes[i].dest)nestado++;
                    }

                    if(apontamPara<2&&estados[estados[src].transicoes[i].dest].transicoes.size() == 2)
                    {
                        if(estados[estados[src].transicoes[i].dest].transicoes[0].dest == estados[src].transicoes[i].dest||estados[estados[src].transicoes[i].dest].transicoes[1].dest == estados[src].transicoes[i].dest)
                        {
                            if(estados[estados[src].transicoes[i].dest].transicoes[0].dest == estados[src].transicoes[i].dest)
                            {
                                regExp02 = estados[estados[src].transicoes[i].dest].transicoes[0].regExp;
                                regExp03 = estados[estados[src].transicoes[i].dest].transicoes[1].regExp;
                                estadoApontaPara = estados[estados[src].transicoes[i].dest].transicoes[1].dest;
                            }
                            else 
                            {
                                regExp02 = estados[estados[src].transicoes[i].dest].transicoes[1].regExp;
                                regExp03 = estados[estados[src].transicoes[i].dest].transicoes[0].regExp;
                                estadoApontaPara = estados[estados[src].transicoes[i].dest].transicoes[0].dest;
                            }
                            regExp01 = estados[src].transicoes[i].regExp;
                            fechaRegularExpression(regExp01);
                            fechaRegularExpression(regExp02);
                            fechaRegularExpression(regExp03);
                            if(regExp01 == "&")regExp01 = "";
                            if(regExp03 == "&")regExp03 = "";
                            
                            //fecha o regExp02
                            parentesesEsq = 0;
                            if(regExp02[0] == '(')parentesesEsq++;
                            for(int posString = 1,sizeStr = regExp02.size()-1;posString<sizeStr&&parentesesEsq>0;posString++)
                                if(regExp02[posString] == '(')parentesesEsq++;
                                else if(regExp02[posString] == ')')parentesesEsq--;
                            if(parentesesEsq == 0&&regExp02.size()>1)
                            {
                                regExp02.insert(0, "(");
                                regExp02.append(")");
                            }
                            parentesesEsq = 0;
                            regExp02.append("*");
                            
                            estadoParaEliminar = estados[src].transicoes[i].dest;
                            
                            //o estado intermediario sera eliminado
                            estados[src].transicoes[i].dest = estadoApontaPara;
                            estados[src].transicoes[i].regExp = regExp01;
                            estados[src].transicoes[i].regExp.append(regExp02);
                            estados[src].transicoes[i].regExp.append(regExp03);
                            changed = true;
                            eliminaEstado(estadoParaEliminar);
                            printGraphInTXT(); 

                        }
                    }
                    else if(apontamPara<2&&estados[estados[src].transicoes[i].dest].transicoes.size() == 1)
                    {
                        regExp01 = estados[src].transicoes[i].regExp;

                        regExp02 = estados[estados[src].transicoes[i].dest].transicoes[0].regExp;

                        
                        if(regExp01 == "&")regExp01 = "";
                        else if(regExp02 == "&")regExp02 = "";
                        
                        if(regExp01.size()>0&&regExp02.size()>0)
                        {
                            fechaRegularExpression(regExp01);
                            fechaRegularExpression(regExp02);
                        }
                        
                        //if(regExp01 == "&")regExp01 = "";
                        //if(regExp02 == "&")regExp02 = "";
                        //o estado intermediario sera eliminado
                        
                        estadoParaEliminar = estados[src].transicoes[i].dest;
                        estados[src].transicoes[i].dest = estados[estados[src].transicoes[i].dest].transicoes[0].dest;
                        estados[src].transicoes[i].regExp = regExp01;
                        estados[src].transicoes[i].regExp.append(regExp02);
                        changed = true;
                        eliminaEstado(estadoParaEliminar);
                        printGraphInTXT(); 

                    }
                }
                if(!changed&&estados[estados[src].transicoes[i].dest].transicoes.size()>0&&estados[src].transicoes[i].dest!=src)
                {
                    changed = passaPorAutomatoRegExp(estados[src].transicoes[i].dest);
                }

            }
            return changed;
        }

        // verificar se uma transição já existe
        bool existsArc(struct transicao trans){
            for (int i=0;i<estados.size();i++){
                for (int j=0;j<estados[i].transicoes.size();j++){
                    struct transicao tmp;
                    if (trans.src == estados[i].transicoes[j].src
                        && trans.dest == estados[i].transicoes[j].dest
                        && trans.regExp == estados[i].transicoes[j].regExp)
                        return true;
                }
            }
            return false;
        }

};

int main(){
    int i;
    bool validade;
    std::vector<Graph> automatos;
    std::vector<std::string> cadeias {"ab", "abb", "bba", "abba"}; 
    //std::vector<Graph> automatos;
    automatos.resize(4);
    std::cout << "\n\n\nQuetao01\n" ;
    automatos[0].createGraph("(a+b)*bb(b+a)*", 0);
    automatos[1].createGraph("(a(b+c))*", 1);
    automatos[2].createGraph("a*b+b*a", 2);
    automatos[3].createGraph("a*b*c*", 3);
    std::cout << "\n\n\nQuetao02\n" ;
    for(int j = 0,m = automatos.size(); j<m; j++)
    {
        std::cout << "automato["<< j <<"].regularExp = " << automatos[j].regularExp << std::endl;
        for(int z = 0, n = cadeias.size(); z<n ; z++)
        {
            std::cout << "\n\ncadeia: " << cadeias[z] <<"--------------\n\n";
            validade = automatos[j].cadeiaValida(cadeias[z]);
            std::cout << "\n\ncadeia: " << cadeias[z] << " : ";
            if(!validade)std::cout <<"NAO EH";
            std::cout << " valida\n\n///////////////////////////\n\n";
        }
    }
    
    std::cout << "Iniciando as remocoes dos arcos epsilon..." << std::endl;
    for (int i=0;i<automatos.size();i++){
        automatos[i].removeEpsilonTrans(i);
    }
    std::cout << "Arcos epsilon removidos!" << std::endl;

    std::cout << "\n\n\nQuetao04\n" ;
    automatos.resize(5);
    automatos[0].createGraph("(a+b)*bb(b+a)*", 0);
    automatos[1].createGraph("(a(b+c))*", 1);
    automatos[2].createGraph("a*b+b*a", 2);
    automatos[3].createGraph("a*b*c*", 3);
    automatos[4].createGraph("(a+b)*bb(b+a)*", 4); 
    automatos[4].estados[0].transicoes.push_back({0,0,"a"});
    
    automatos[4].estados[1].transicoes.push_back({1,1,"b"});
    automatos[4].estados[1].transicoes.push_back({1,1,"a"});
    for(int j = 0,m = automatos.size(); j<m; j++)
    {
        std::cout << "automato["<< j <<"].regularExp = " << automatos[j].geraRegExp() << std::endl;
        
    }
    std::cout << "\n\nFim da Quetao04\n////////////////////////////////////////\n\n" ;

    

    std::cout << "Finalizado" << std::endl;
    std::cout << std::endl;
/*     automatos.resize(5); // caso simples de teste
    automatos[4].estados.resize(3);
    // estado 0
    automatos[4].estados[0].valida = false;
    struct transicao trans;
    trans.src=0; trans.dest=0; trans.regExp="0";
    automatos[4].estados[0].transicoes.push_back(trans);
    trans.src=0; trans.dest=1; trans.regExp="&";
    automatos[4].estados[0].transicoes.push_back(trans);
    // estado 1
    automatos[4].estados[1].valida = false;
    trans.src=1; trans.dest=1; trans.regExp="1";
    automatos[4].estados[1].transicoes.push_back(trans);
    trans.src=1; trans.dest=2; trans.regExp="&";
    automatos[4].estados[1].transicoes.push_back(trans);
    // estado 2
    automatos[4].estados[2].valida = true;
    trans.src=2; trans.dest=2; trans.regExp="2";
    automatos[4].estados[2].transicoes.push_back(trans);

    // removeEpsilonTransitions
    automatos[4].removeEpsilonTrans();  */

    std::cin.get();
    return 0;
}
