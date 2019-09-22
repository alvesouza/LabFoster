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
    std::vector<transicao> transicoes;
};

class Graph{
    public:
        std::vector<estado> estados;
        std::string regularExp;
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
            fileName.append(std::to_string(questNumber));
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
            estados.push_back(aux);//adciona estado 0
            aux.transicoes.resize(0);
            aux.valida = true;
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
                    myFile << " " << i;
            }
            myFile << "\n\tnode [shape = circle];\n\t";
            for(int i = 0;i < numberStates;i++)
            {
                for(int j = 0, m = estados[i].transicoes.size(); j<m; j++)
                    {
                        myFile << i <<" -> " << estados[i].transicoes[j].dest << " [ label = \"" << estados[i].transicoes[j].regExp <<"\" ];\n";
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
                std::cout << "estado: "<< possiveisEstados[i];
                
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

    private:
        std::ofstream myFile;
        std::string fileName;
        int etapa;

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
                if(estados[estadoAtual].transicoes[i].regExp[0] == '&')//passará sem, se mover na cadeia
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

};

int main(){
    int i;
    bool validade;
    std::vector<Graph> automatos;
    std::vector<std::string> cadeias {"ab", "abb", "bba", "abba"}; 
    //std::vector<Graph> automatos;
    automatos.resize(4);
    automatos[0].createGraph("(a+b)*bb(b+a)*", 0);
    automatos[1].createGraph("(a(b+c))*", 1);
    automatos[2].createGraph("a*b+b*a", 2);
    automatos[3].createGraph("a*b*c*", 3);

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

    std::cout << "Finalizado" << std::endl;
    std::cin >> i;
    return 0;
}
