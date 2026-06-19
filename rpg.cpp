// ------------ BIBLIOTECAS -----------------
#include <iostream>   // Entrada e saida padrao
#include <vector>     // Vetor dinamico
#include <memory>     // Ponteiros inteligentes
#include <string>     // Manipulacao de strings
#include <map>        // Mapa chave-valor
#include <functional> // Funcoes de ordem superior
#include <stdexcept>  // Excecoes padrao
#include <algorithm>  // Algoritmos de busca e ordenacao
#include <random>     // Geracao de numeros aleatorios
#include <fstream>    // Leitura e escrita em arquivos
#include <sstream>    // Manipulacao de strings como stream
#include <regex>      // Expressoes regulares para parsing do JSON de save
using namespace std;

// ---------------- FUNCOES UTILITARIAS -----------------

// Gera um numero inteiro aleatorio no intervalo [inicio, fim] (inclusivo nos dois lados)
// Usa o Mersenne Twister (mt19937) para maior qualidade de aleatoriedade
int randint(int inicio, int fim){
    return inicio + rand() % (fim - inicio + 1);
}

// Escolhe aleatoriamente um elemento de um vetor de shared_ptr
// Retorna nullptr se a lista estiver vazia
template <typename T>
shared_ptr<T> choice(const vector<shared_ptr<T>> &lista){
    if (lista.empty())
        return nullptr;
    int indice = randint(0, (int)lista.size() - 1); // Sorteia o indice
    return lista[indice];
}

// ------------- DECLARACOES ANTECIPADAS ------------------
// Necessarias para que classes que se referenciam mutuamente possam ser compiladas
class Item;
class Jogador;
class Inimigo;
class Raca;
class MeioElfo;
class Humano;
class Tiefling;
class Bruxo;
class Ladrao;
class Artifice;
class Guerreiro;
class Logger;
class GerenciadorDeJogo;
class CriacaoItens;
class CriacaoInimigos;

// ------------- CLASSE INVENTARIO ---------------
// Gerencia os itens carregados por um jogador, com controle de capacidade por espaco ocupado
class Inventario{
public:
    vector<shared_ptr<Item>> itens; // Lista de itens dentro do inventario
    int capacidade;

    Inventario(int capacidade = 20);                      // Limite maximo do inventario
    bool adicionarItem(shared_ptr<Item> item);            // Tenta adicionar item; lanca excecao se inventario cheio
    bool removerItem(const string &nomeItem);             // Remove item pelo nome; lanca excecao se nao encontrado
    void listarItens();                                   // Exibe todos os itens e espaco ocupado/total
    shared_ptr<Item> buscarItens(const string &nomeItem); // Retorna o item pelo nome; lanca excecao se ausente
};

// --------------- CLASSE RACA (BASE) -----------------
// Classe base para as racas dos jogadores; define bonus de atributos aplicados no construtor das subclasses
class Raca{
public:
    int bonusCarisma;
    int bonusInteligencia;
    int bonusAgilidade;

    Raca(int bonusCarisma = 0, int bonusInteligencia = 0, int bonusAgilidade = 0){
        this->bonusCarisma = bonusCarisma;
        this->bonusInteligencia = bonusInteligencia;
        this->bonusAgilidade = bonusAgilidade;
    }

    virtual string nomeClasse() const{
        return "Raca";
    }

    // Mudar pra ele mostrar o nome da Raca e nao o endereco de memoria de onde ta
    virtual string str() const{
        return nomeClasse();
    }

    virtual ~Raca() = default;
};

// ------------ RACAS CONCRETAS -----------------
// Cada subclasse define os bonus especificos da raca via construtor da classe base Raca

// Racas
class MeioElfo : public Raca{
public:
    MeioElfo() : Raca(4, 2, 2) {}
    string nomeClasse() const override { return "MeioElfo"; }
};

class Humano : public Raca{
public:
    Humano() : Raca(2, 2, 2) {}
    string nomeClasse() const override { return "Humano"; }
};

class Tiefling : public Raca{
public:
    Tiefling() : Raca(4, 2, 0) {}
    string nomeClasse() const override { return "Tiefling"; }
};

// ------------- CLASSE ITEM (BASE ABSTRATA) ----------------
// Define a interface comum para todos os itens do jogo (armas, armaduras, consumiveis)
// equipar/desequipar sao metodos abstratos implementados por cada subclasse
class Item{
public:
    string nomeItem;
    int valor;
    int espaco;
    int quantidade;

    Item(string nomeItem, int valor, int espaco){
        this->nomeItem = nomeItem;
        this->valor = valor;
        this->espaco = espaco;
        this->quantidade = 1;
    }

    virtual void equipar(shared_ptr<Jogador> jogador) = 0;
    virtual void desequipar(shared_ptr<Jogador> jogador) = 0;
    virtual string str() const{
        return nomeItem;
    }
    virtual ~Item() = default;
};

// -------------- CLASSE JOGADOR (BASE ABSTRATA) ----------------------
// Classe base para todos os tipos de jogadores (Bruxo, Ladrao, Artifice, Guerreiro)
// Herda de enable_shared_from_this para poder gerar shared_ptr de si mesmo com seguranca
// Os atributos de nome, level e hp sao privados e acessados via getters/setters
class Jogador : public enable_shared_from_this<Jogador>{
private:
    string __nome;
    int __level;
    int __hp;
    int __hpMax;

public:
    int xp;
    int xpMax;
    bool morto;
    shared_ptr<Raca> raca;

    Inventario inventario;
    shared_ptr<Item> armaEquipada;
    int armaduraBase;
    shared_ptr<Item> armaduraEquipada;
    int defesaTotal;
    string classe;
    int dano;
    int mana;
    int furtividade;

    Jogador(string nome, int level, shared_ptr<Raca> raca, int hp, int xp, int armadura = 0)
        : inventario(){
        this->__nome = nome;
        this->__level = level;
        this->__hp = hp;
        this->__hpMax = hp;
        this->xp = xp;
        this->xpMax = 100;
        this->morto = false;
        this->raca = raca;
        this->armaEquipada = nullptr;
        this->armaduraBase = armadura;
        this->armaduraEquipada = nullptr;
        this->defesaTotal = 0;
        this->dano = 0;
        this->mana = 0;
        this->furtividade = 0;
    }

    // Declarando os getters
    string getNome() const{
        return __nome;
    }

    int getLevel() const{
        return __level;
    }

    int getHP() const{
        return __hp;
    }

    int getHPMax() const{
        return __hpMax;
    }

    int getArmadura();

    // Declarando os setters
    void setNome(string nome){
        this->__nome = nome;
    }

    void setLevel(int level){
        this->__level = level;
    }

    void setHP(int hp){
        this->__hp = hp;
    }

    void setHPMax(int hpMax){
        this->__hpMax = hpMax;
    }

    virtual void exibirStatus() = 0;

    virtual void receberDano(int dano, shared_ptr<void> matador = nullptr){ // Passa o jogador que matar o inimigo (o que der o ultimo hit) para ganhar o xp
        this->__hp -= dano;
        if (this->__hp <= 0){
            this->__hp = 0;
            this->morto = true;

            cout << "Game Over para " << this->__nome << endl;
        }
    }

    bool curar(int cura){
        if (this->__hp == this->__hpMax){
            cout << "Nao é possivel se curar, vida já esta cheia" << endl;
            return false;
        }

        else if (this->__hp <= 0){
            cout << this->__nome << " esta morto, ache um orbe da resurreicao" << endl;
            return false;
        }

        this->__hp += cura;
        if (this->__hp > this->__hpMax){
            this->__hp = this->__hpMax;
            cout << "Vida totalmente restaurada" << endl;
        }
        return true;
    }

    bool ganharXP(int experiencia);

    virtual int atacar(shared_ptr<Jogador> alvo) = 0;
    virtual int atacar(shared_ptr<Inimigo> alvo) = 0;

    virtual map<string, string> serializaDicionario(); // Cria um ponto de save

    static shared_ptr<Jogador> desserializaDicionario(const map<string, string> &dados); // Volta no save (reconstroi o objeto do mesmo jeito que foi construido no ponto de save)

    virtual string nomeClasse() const = 0;

    virtual string str() const{
        stringstream ss;
        ss << "Classe: " << nomeClasse() << ", Nome: " << __nome << ", Raca: " << raca->str()
           << ", Level: " << __level << ", HP: " << __hp << "/" << __hpMax << ", XP: " << xp << "/" << xpMax;
        return ss.str();
    }

    virtual ~Jogador() = default;
};

// -------------- CLASSE INIMIGO (BASE ABSTRATA) ----------------
// Classe base para todos os inimigos do jogo; define HP, dano, recompensas e logica de receber dano
// O metodo atacar() e abstrato e deve ser implementado por cada tipo de inimigo (Dragao, Basilisco, Saqueadores)
class Inimigo : public enable_shared_from_this<Inimigo>{ // Classe so pra inimigo
private:
    string __nome;
    int __hp;
    int __hpMax;

public:
    int dano;
    int xpRecompensa;
    int ouroRecompensa;
    bool morto;
    int armadura;
    string estadoMonstro;

    Inimigo(string nome, int hp, int dano, int xpRecompensa, int ouroRecompensa){
        this->__nome = nome;
        this->__hp = hp;
        this->__hpMax = hp;
        this->dano = dano;
        this->xpRecompensa = xpRecompensa;
        this->ouroRecompensa = ouroRecompensa;
        this->morto = false;
        this->armadura = 1;
    }

    // Getters dos inimigos
    string getNome() const{
        return __nome;
    }

    int getArmadura() const{
        return armadura;
    }

    int getHP() const{
        return __hp;
    }

    int getHPMax() const{
        return __hpMax;
    }

    // Setters dos inimigos
    void setNome(string nome){
        this->__nome = nome;
    }

    void setHP(int hp){
        this->__hp = hp;
    }

    void setHPMax(int hpMax){
        this->__hpMax = hpMax;
    }

    virtual int atacar(shared_ptr<Jogador> alvo) = 0; // Aonde os inimigos irao herdar o atacar

    void droparRecompensas(shared_ptr<Jogador> jogador); // Funcao que ativa caso o inimigo morra, ai ele libera a recompensas para os jogadores

    void receberDano(int dano, shared_ptr<Jogador> jogador = nullptr){
        this->__hp -= dano;
        if (this->__hp <= 0){
            this->__hp = 0;
            this->morto = true;
            cout << this->getNome() << " foi derrotado!" << endl;
            if (jogador != nullptr){
                this->droparRecompensas(jogador);
            }
        }
    }

    virtual ~Inimigo() = default;
};

// -------------- CLASSE LOGGER (SINGLETON) -----------------
// Registra e exibe mensagens de log de toda a partida
// Implementado como Singleton: so existe uma instancia em todo o jogo,
// garantindo que todos os eventos sejam registrados no mesmo lugar
class Logger{
private:
    static Logger *_instancia; // Recebendo None como default
    bool _inicializado;

    Logger()
    {
        _inicializado = false;
        if (this->_inicializado){  // Ve se ja foi iniciado antes
            return;                 // Se sim, sai sem fazer nada e impede o init setar de novo o ouro, xp e nivel pra 0
        }
        // Se nao, seta nivel em 1 e o resto em 0
        this->logs = {};
        this->_inicializado = true;
    }

public:
    vector<string> logs;

    static Logger &instancia(){
        if (_instancia == nullptr){
            _instancia = new Logger();
        }
        return *_instancia;
    }

    void registrarMensagem(string msg){
        this->logs.push_back(msg);
    }

    void exibirLogs(){
        for (string log : this->logs){
            cout << log << endl;
        }
    }

    void log(string msg){
        cout << msg << endl;
        this->logs.push_back(msg);
    }
};

Logger *Logger::_instancia = nullptr;

// -------------- CLASSE ARMA -----------------
// Item equipavel que aumenta o dano do jogador; ao equipar, e removida do inventario
// e ao desequipar, retorna ao inventario
class Arma : public Item{
public:
    int dano;

    Arma(string nomeItem, int valor, int espaco, int dano) : Item(nomeItem, valor, espaco){
        this->quantidade = 1;
        this->dano = dano;
    }

    void equipar(shared_ptr<Jogador> jogador) override{
        if (jogador->armaEquipada == nullptr){               // Se a arma nao tiver equipada
            jogador->armaEquipada = shared_from_this_item(); // Pego a arma
            jogador->dano = this->dano;
            auto &itens = jogador->inventario.itens;
            itens.erase(remove_if(itens.begin(), itens.end(), [&](shared_ptr<Item> item)
                                  { return item.get() == this; }),
                        itens.end()); // Remove a arma do inventario
            cout << jogador->getNome() << " equipou a " << this->nomeItem << endl;
        }
        else{
            cout << jogador->getNome() << ", ja esta com a " << this->nomeItem << " equipada!" << endl;
        }
    }

    void desequipar(shared_ptr<Jogador> jogador) override{
        if (jogador->armaEquipada != nullptr){ // Se a arma tiver equipada
            shared_ptr<Item> item = jogador->armaEquipada;
            jogador->armaEquipada = nullptr;         // Guarda a arma
            jogador->inventario.adicionarItem(item); // Coloca o item de volta no inventario, voltando a ocupar espaco
            jogador->dano = 0;
            cout << jogador->getNome() << " largou a " << this->nomeItem << endl;
        }
        else{
            cout << jogador->getNome() << ", ja guardou a " << this->nomeItem << "!" << endl;
        }
    }

    // Passar pra string
    string str() const override{
        stringstream ss;
        ss << this->nomeItem << ", " << this->valor << ", " << this->espaco << ". " << this->dano;
        return ss.str();
    }

private:
    shared_ptr<Item> shared_from_this_item(){
        return shared_ptr<Item>(this, [](Item *) {});
    }
};

// --------------- CLASSE ARMADURA ----------------
// Item equipavel que aumenta a defesa total do jogador; funciona igual a Arma
// (sai do inventario ao equipar e volta ao desequipar)
class Armadura : public Item{
public:
    int defesa;

    Armadura(string nomeItem, int valor, int espaco, int defesa) : Item(nomeItem, valor, espaco){
        this->quantidade = 1;
        this->defesa = defesa;
    }

    void equipar(shared_ptr<Jogador> jogador) override{
        if (jogador->armaduraEquipada == nullptr){               // Se o JOGADOR estiver sem arma, ele roda esse bloco e nao o self (que nesse caso seria a propria arma)
            jogador->armaduraEquipada = shared_from_this_item(); // Coloco a armadura
            jogador->defesaTotal += this->defesa;
            auto &itens = jogador->inventario.itens;
            itens.erase(remove_if(itens.begin(), itens.end(), [&](shared_ptr<Item> item)
                                  { return item.get() == this; }),
                        itens.end()); // Zero o valor que ele ocupa no inventario (ou seja, tiro a armadura do inventario para coloca-la)
            cout << jogador->getNome() << " vestiu sua " << this->nomeItem << endl;
        }
        else{
            cout << jogador->getNome() << ", ja esta com a " << this->nomeItem << " equipada!" << endl;
        }
    }

    void desequipar(shared_ptr<Jogador> jogador) override{
        if (jogador->armaduraEquipada != nullptr){ // Mesma coisa que para colocar a arma
            shared_ptr<Item> item = jogador->armaduraEquipada;
            jogador->armaduraEquipada = nullptr;     // Tira a armadura
            jogador->defesaTotal -= this->defesa;    // Perde a defesa extra
            jogador->inventario.adicionarItem(item); // Coloca o item de volta no inventario, voltando a ocupar espaco
            cout << jogador->getNome() << " tirou a " << this->nomeItem << endl;
        }
        else{
            cout << jogador->getNome() << ", ja esta sem a " << this->nomeItem << "!" << endl;
        }
    }

    string str() const override{
        stringstream ss;
        ss << this->nomeItem << ", " << this->valor << ", " << this->espaco << ", " << this->defesa;
        return ss.str();
    }

private:
    shared_ptr<Item> shared_from_this_item(){
        return shared_ptr<Item>(this, [](Item *) {});
    }
};

// Definido apos Armadura pois precisa de dynamic_pointer_cast<Armadura> para calcular o bonus de defesa
// Agora que Armadura existe, a funcao getArmadura funciona normalmente em tempo de compilacao.
int Jogador::getArmadura(){
    int bonus = 0;
    if (this->armaduraEquipada != nullptr){
        shared_ptr<Armadura> armadura = dynamic_pointer_cast<Armadura>(this->armaduraEquipada);
        if (armadura != nullptr)
            bonus = armadura->defesa;
    }
    return this->armaduraBase + bonus;
}

Inventario::Inventario(int capacidade){  // Limite maximo do inventario
    this->itens = {};                    // Lista de itens dentro do inventario
    this->capacidade = capacidade;
}

bool Inventario::adicionarItem(shared_ptr<Item> item){
    int totalEspaco = 0;
    for (auto i : this->itens){                 // Vai percorrendo o valor do espaco dos itens e salvando em totalEspaco
        totalEspaco += (i->quantidade + 8) / 9; // Vai separando a cada 9 itens estacados no inventario, ele usa mais um espaco para guardar
    }
    // Ele ve se a quantidade do proximo item + o espaco que ja ta ocupado vai ser maior que a capacidade maxima
    if (totalEspaco + (item->quantidade + 8) / 9 > this->capacidade){
        throw runtime_error("Inventario Lotado! Capacidade maxima: " + to_string(totalEspaco) + "/" + to_string(this->capacidade));
    }
    else{
        this->itens.push_back(item); // Coloca o item recebido no vetor de itens
        cout << "Item " << item->nomeItem << " adicionado com sucesso" << endl;
        return true;
    }
}

bool Inventario::removerItem(const string &nomeItem){
    // Ele faz isso enquanto procura o item na lista de itens
    for (auto it = this->itens.begin(); it != this->itens.end(); ++it){
        shared_ptr<Item> item = *it;
        if (item->nomeItem == nomeItem){    // Se o nome do item chamado for o mesmo nome do item que ta no inventario do jogador
            this->itens.erase(it);          // Ele remove o item
            cout << nomeItem << " removido do inventario" << endl;
            return true;
        }
    }
    throw runtime_error("O item " + nomeItem + " nao foi localizado ou nao existe");

    // Mesmo que o item nao exista, ele ainda vai pegar o nome do item digitado, salvar na variavel "nomeItem" e depois comparar pra ver se ele existe,
    // ai como ja ta salvo na variavel, se nao achar, ele printa o item que ta salvo na variavel e retorna a mensagem de erro
}

void Inventario::listarItens(){
    cout << "Inventario: " << endl;
    int totalEspaco = 0;
    int i = 1;
    for (auto item : this->itens){                                                                                        // Vai buscando o item na variavel itens
        int pilhas = (item->quantidade + 8) / 9;                                             // Faz uma pilha de itens, salvando quantos itens tem empilhados, tipo, se tem 10 unidades de um item, ele empilha 9 e sobre 1, ai...
        totalEspaco += pilhas;                                                               // Ja ocupa mais 1 espaco no inventario a cada 9 estacados
        cout << i << ". " << item->nomeItem << ", quantidade: " << item->quantidade << endl; // Printando todos os itens no inventario
        i += 1;
    }
    cout << " \nEspaco total: " << totalEspaco << "/" << this->capacidade << endl;
}

shared_ptr<Item> Inventario::buscarItens(const string &nomeItem){
    for (auto item : this->itens){
        if (item->nomeItem == nomeItem){
            return item;
        }
    }
    throw runtime_error("Item " + nomeItem + " nao encontrado!");
}

// ------------- CLASSE CONSUMIVEIS (BASE) ---------------------
// Classe base para todos os itens consumiveis (pocoes, orbes)
// usarItem() e sobrecarregado para diferentes alvos: jogador, dois jogadores, ou inimigo
class Consumiveis : public Item{ // Herdando de Item e de ABC (para fazer o usar item ser um metodo abstrato)
public:
    Consumiveis(string nomeItem, int valor, int espaco, int quantidade) : Item(nomeItem, valor, espaco){
        this->quantidade = quantidade;
    }

    void equipar(shared_ptr<Jogador> jogador) override{
    }

    void desequipar(shared_ptr<Jogador> jogador) override{
    }

    virtual bool usarItem(shared_ptr<Jogador> usuario, shared_ptr<Jogador> alvo) { return false; }
    virtual bool usarItem(shared_ptr<Jogador> usuario) { return false; }
    virtual bool usarItem(shared_ptr<Jogador> usuario, shared_ptr<Inimigo> alvo) { return false; }

    string str() const override{ // Usando polimorfismo
        stringstream ss;
        ss << this->nomeItem << ", " << this->valor << ", " << this->espaco << ". " << this->quantidade;
        return ss.str();
    }
};

// --------------- CLASSES DE JOGADORES (CONCRETAS) ---------------
// Cada classe define atributos proprios e implementa atacar() com comportamento unico
// Herdam todos os atributos e metodos da classe base Jogador via heranca

// Classes
class Bruxo : public Jogador{
public:
    int carisma;

    Bruxo(string nome, int nivel, shared_ptr<Raca> Raca, int hp = 150, int xp = 0)
        : Jogador(nome, nivel, Raca, hp, xp){
        this->classe = "Bruxo";
        this->mana = 100;
        this->carisma = 20 + Raca->bonusCarisma;
        // Nao colocado para equipar arma pois ele tem foco arcano que anda com ele
    }

    int atacar(shared_ptr<Jogador> alvo) override;
    int atacar(shared_ptr<Inimigo> alvo) override;

    void exibirStatus() override{
        cout << "Classe: " << this->classe << ", Nome: " << this->getNome() << ", Raca: " << this->raca->str() << ", Level: " << this->getLevel() << ", HP: " << this->getHP() << "/" << this->getHPMax() << ", XP: " << this->xp << "/" << this->xpMax << endl;
    }

    string nomeClasse() const override { return "Bruxo"; }
};

class Ladrao : public Jogador{
public:
    int agilidade;

    Ladrao(string nome, int nivel, shared_ptr<Raca> Raca, int hp = 170, int xp = 0)
        : Jogador(nome, nivel, Raca, hp, xp){
        this->classe = "Ladrao";
        this->agilidade = 15 + Raca->bonusAgilidade;
        this->furtividade = 10;

        // Atributos movidos para a classe base Jogador, comentados aqui apenas como referencia historica:
        // this->defesaTotal = 0;
        // this->armaEquipada = nullptr;
        // this->armaduraEquipada = nullptr;
    }

    int atacar(shared_ptr<Jogador> alvo) override;
    int atacar(shared_ptr<Inimigo> alvo) override;

    void exibirStatus() override{
        cout << "Classe: " << this->classe << ", Nome: " << this->getNome() << ", Raca: " << this->raca->str() << ", Level: " << this->getLevel() << ", HP: " << this->getHP() << "/" << this->getHPMax() << ", XP: " << this->xp << "/" << this->xpMax << endl;
    }

    string nomeClasse() const override { return "Ladrao"; }
};

class Artifice : public Jogador{
public:
    int inteligencia;

    Artifice(string nome, int nivel, shared_ptr<Raca> Raca, int hp = 150, int xp = 0)
        : Jogador(nome, nivel, Raca, hp, xp){
        this->classe = "Artifice";
        this->mana = 100;
        this->inteligencia = 20 + Raca->bonusInteligencia;
    }

    shared_ptr<Item> criarItem(string tipo, int quantidade = 1);
    int atacar(shared_ptr<Jogador> alvo) override;
    int atacar(shared_ptr<Inimigo> alvo) override;

    void exibirStatus() override{
        cout << "Classe: " << this->classe << ", Nome: " << this->getNome() << ", Raca: " << this->raca->str() << ", Level: " << this->getLevel() << ", HP: " << this->getHP() << "/" << this->getHPMax() << ", XP: " << this->xp << "/" << this->xpMax << endl;
    }

    string nomeClasse() const override { return "Artifice"; }
};

class Guerreiro : public Jogador{
public:
    int forca;

    Guerreiro(string nome, int nivel, shared_ptr<Raca> Raca, int hp = 250, int xp = 0)
        : Jogador(nome, nivel, Raca, hp, xp, 5){
        this->classe = "Guerreiro";
        this->forca = 20 + Raca->bonusAgilidade;
    }

    int atacar(shared_ptr<Jogador> alvo) override;
    int atacar(shared_ptr<Inimigo> alvo) override;

    void exibirStatus() override{
        cout << "Classe: " << this->classe << ", Nome: " << this->getNome() << ", Raca: " << this->raca->str() << ", Level: " << this->getLevel() << ", HP: " << this->getHP() << "/" << this->getHPMax() << ", XP: " << this->xp << "/" << this->xpMax << endl;
    }

    string nomeClasse() const override { return "Guerreiro"; }
};

// ------------------- INIMIGOS CONCRETOS -----------------
// Cada inimigo define HP, dano, recompensas e a logica propria de ataque via atacar()
// estadoMonstro e a chave usada no Singleton GerenciadorDeJogo para marcar se o inimigo foi derrotado

class Dragao : public Inimigo{
public:
    Dragao() : Inimigo("Dragao", 1000, 30, 100, 200){
        this->armadura = 7;
        this->estadoMonstro = "dragaoMorto"; // Ve no sinlgeton se ele ta morto ou nao
    }

    int atacar(shared_ptr<Jogador> alvo) override;
};

class Basilisco : public Inimigo{
public:
    Basilisco() : Inimigo("Basilisco", 700, 25, 80, 120){
        this->armadura = 5;
        this->estadoMonstro = "dragaoMorto";
    }

    int atacar(shared_ptr<Jogador> alvo) override;
};

class Saqueadores : public Inimigo{
public:
    Saqueadores() : Inimigo("Saqueadores", 300, 15, 30, 100){
        this->armadura = 1;
        this->estadoMonstro = "dragaoMorto";
    }

    int atacar(shared_ptr<Jogador> alvo) override;
};

// --------------- POCOES ----------------
// Subclasses de Consumiveis; cada uma sobrescreve usarItem() com efeito especifico
// PocaoVida cura; PocaoMana recupera mana; PocaoDanoElemental e suas subclasses causam dano

// Pocoes
class PocaoVida : public Consumiveis{
public:
    PocaoVida(int quantidade = 0)
        : Consumiveis("Pocao de Vida", 60, 1, quantidade) {}

    bool usarItem(shared_ptr<Jogador> usuario, shared_ptr<Jogador> alvo) override{
        if (this->quantidade > 0){
            if (alvo->morto || usuario->morto){
                if (usuario != alvo){
                    if (alvo->morto){
                        cout << alvo->getNome() << " esta morto e nao pode receber cura" << endl;
                        return false;
                    }
                    else if (usuario->morto){
                        cout << usuario->getNome() << " esta morto e nao pode curar " << alvo->getNome() << endl;
                        return false;
                    }
                }
                else{
                    cout << usuario->getNome() << " esta morto e nao pode se curar" << endl;
                    return false;
                }
            }

            alvo->curar(20);       // Chama a funcao curar la de Jogador e cura um jogador
            this->quantidade -= 1; // Diminui a quantidade dessa pocao
            if (usuario->getNome() == alvo->getNome()){
                cout << usuario->getNome() << " se curou! || HP: " << usuario->getHP() << "/" << usuario->getHPMax() << endl;
                return true;
            }

            cout << usuario->getNome() << " usou " << this->nomeItem << " em " << alvo->getNome() << " com sucesso! || HP: " << alvo->getHP() << "/" << alvo->getHPMax() << endl; // Nao precisa herdar tudo de Jogador, pois o alvo ja faz isso, so precisa chamar o getter por conta do nome ser privado
            return true;                                                                                                                                                          // Validar que deu certo usar o item
        }
        else{
            throw runtime_error("Voce nao tem mais esta pocao!");
        }
    }
};

class PocaoMana : public Consumiveis{
public:
    PocaoMana(int quantidade = 0)
        : Consumiveis("Pocao de Mana", 60, 1, quantidade) {}

    bool usarItem(shared_ptr<Jogador> usuario) override{
        if (this->quantidade > 0){
            if (usuario->morto){
                cout << usuario->getNome() << " esta morto e nao pode recuperar mana" << endl;
                return false;
            }
            usuario->mana += 20; // Aumenta a mana do jogador
            if (usuario->mana > 100){
                usuario->mana = 100;
            }
            this->quantidade -= 1; // Diminui a quantidade dessa pocao
            cout << usuario->getNome() << " usou " << this->nomeItem << "! || Mana: " << usuario->mana << endl;
            return true; // Validar que deu certo usar o item
        }
        else{
            throw runtime_error("Voce nao tem mais esta pocao!");
        }
    }
};

class PocaoDanoElemental : public Consumiveis{
public:
    int dano;

    PocaoDanoElemental(string nomeItem, int valor, int espaco, int quantidade, int dano)
        : Consumiveis(nomeItem, valor, espaco, quantidade){
        this->dano = dano;
    }

    bool usarItem(shared_ptr<Jogador> usuario, shared_ptr<Jogador> alvo) override{
        if (this->quantidade > 0){
            if (usuario->morto){
                cout << usuario->getNome() << " esta morto e nao pode usar item" << endl;
                return false;
            }
            this->quantidade -= 1;         // Diminui a quantidade dessa pocao
            alvo->receberDano(this->dano); // Recebe o dano de cada pocao especifica
            cout << usuario->getNome() << " usou " << this->nomeItem << " em " << alvo->getNome() << "! || " << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << " HP" << endl;

            return true; // Validar que deu certo usar o item
        }
        else{
            throw runtime_error("Voce nao tem mais esta pocao!\n");
        }
    }

    bool usarItem(shared_ptr<Jogador> usuario, shared_ptr<Inimigo> alvo) override{
        if (this->quantidade > 0){
            if (usuario->morto){
                cout << usuario->getNome() << " esta morto e nao pode usar item\n"
                     << endl;
                return false;
            }
            this->quantidade -= 1;                  // Diminui a quantidade dessa pocao
            alvo->receberDano(this->dano, usuario); // Recebe o dano de cada pocao especifica
            cout << usuario->getNome() << " usou " << this->nomeItem << " em " << alvo->getNome() << "! || " << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << " HP" << endl;

            return true; // Validar que deu certo usar o item
        }
        else{
            throw runtime_error("Voce nao tem mais esta pocao!");
        }
    }
};

// Pocoes de dano separadas por elemento; cada uma define nome, valor, espaco e dano proprios
// Separando as pocoes de dano por elementos
class DanoFogo : public PocaoDanoElemental{
public:
    DanoFogo(int quantidade = 0)
        : PocaoDanoElemental("Pocao de Fogo", 50, 1, quantidade, 50) {} // Setando cada valor defaut da pocao
};

class DanoGelo : public PocaoDanoElemental{
public:
    DanoGelo(int quantidade = 0)
        : PocaoDanoElemental("Pocao de Gelo", 40, 1, quantidade, 30) {}
};

class DanoRelampago : public PocaoDanoElemental{
public:
    DanoRelampago(int quantidade = 0)
        : PocaoDanoElemental("Pocao de Relampago", 60, 2, quantidade, 70) {}
};

class DanoRadiante : public PocaoDanoElemental{
public:
    DanoRadiante(int quantidade = 0)
        : PocaoDanoElemental("Pocao Radiante", 999, 1, quantidade, 50) {}
};

class DanoVenenoso : public PocaoDanoElemental{
public:
    DanoVenenoso(int quantidade = 0)
        : PocaoDanoElemental("Pocao Venenosa", 100, 1, quantidade, 40) {}
};

class OrbeRessureicao : public Consumiveis{
public:
    OrbeRessureicao(int quantidade = 0)
        : Consumiveis("Orbe da Ressureicao", 1000, 3, quantidade) {}

    bool usarItem(shared_ptr<Jogador> usuario, shared_ptr<Jogador> alvo) override{
        if (alvo != usuario){
            if (usuario->morto){
                cout << usuario->getNome() << " tambem esta morto e nao pode ressucitar " << alvo->getNome() << endl;
                return false;
            }
            if (usuario->morto == false){
                cout << alvo->getNome() << " ainda esta vivo" << endl;
                return false;
            }
            if (this->quantidade > 0){
                alvo->morto = false;
                alvo->setHP(1); // Setando o hp de volta pra 1 pra nao cair na malha fina do curar
                this->quantidade -= 1;
                cout << alvo->getNome() << " foi ressucitado por " << usuario->getNome() << " || " << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << endl;
                return true;
            }
            else{
                throw runtime_error("Voce nao tem mais orbes!");
            }
        }
        else{
            cout << "Nao é permitido usar o orbe em si mesmo" << endl;
            return false;
        }
    }
};

// ------------- ITEM GENERICO -----------------
// Item sem funcionalidade de equipar/desequipar; usado para itens inúteis como a Pedra Inutil
// criada pelo Artifice quando falha ao criar algo util
class ItemGenerico : public Item{
public:
    ItemGenerico(string nomeItem, int valor = 0, int espaco = 1, int quantidade = 1)
        : Item(nomeItem, valor, espaco){
        this->quantidade = quantidade;
    }

    void equipar(shared_ptr<Jogador> jogador) override{
    }

    void desequipar(shared_ptr<Jogador> jogador) override{
    }

    string str() const override{
        return this->nomeItem + " inutil";
    }
};

// --------------- SINGLETON: GERENCIADOR DE JOGO -------------------
// Centraliza o estado global da partida: ouro do grupo, XP acumulado, nivel, lista de jogadores e inimigos
// e mapa de estado (quais inimigos foram derrotados, quantos itens foram comprados/vendidos)
// Implementado como Singleton: garante que todas as classes (Masmorra, Mercadao, QuadroMissoes)
// leem e escrevem no mesmo objeto

// Singleton (vai ler e escrever tudo no mesmo lugar, seja na masmorra, mercado ou quadro de missao)
class GerenciadorDeJogo{
private:
    static GerenciadorDeJogo *_instancia; // Ou a instancia recebe gerenciador de jogo ou None
    bool _inicializado;

    GerenciadorDeJogo(){
        _inicializado = false;
        if (this->_inicializado){ // Ve se ja foi iniciado antes
            // Se sim, sai sem fazer nada e impede o init setar de novo o ouro, xp e nivel pra 0
            return;
        }
        // Se nao, seta nivel em 1 e o resto em 0
        this->nivel = 1;
        this->ouro = 0;
        this->xp = 0;
        this->estado = {{"dragaoMorto", false}, // Setando os bichos como vivos
                        {"basiliscoMorto", false},
                        {"saqueadores", false},
                        {"itensVendidos", 0},   // E como nada vendido
                        {"itensComprados", 0}}; // E nem comprado

        this->_inicializado = true; // E inicia a instancia
    }

public:
    int nivel;
    int ouro;
    int xp;
    map<string, int> estado;
    vector<shared_ptr<Jogador>> jogadores; // Cria uma lista de jogadores
    vector<shared_ptr<Inimigo>> inimigos;  // Cria uma lista de inimigos

    static GerenciadorDeJogo &instancia(){ // Serve pra instanciar um, objeto ou classe
        if (_instancia == nullptr){
            _instancia = new GerenciadorDeJogo();
        }
        return *_instancia; // Cria e retorna a instancia
    }

    void adicionarOuro(int quantidade){
        this->ouro += quantidade; // Cofrinho do grupo
    }

    void comprarItem(shared_ptr<Item> item){
        if (this->ouro < item->valor){ // Ve se tem grana pra poder comprar coisas
            throw runtime_error("Ta liso paizao? Tu so tem " + to_string(this->ouro) + " e a " + item->nomeItem + " custa " + to_string(item->valor));
        }
        this->ouro -= item->valor;
    }

    void adicionarXP(int xp){
        this->xp += xp;
    }

    void subirNivel(){
        this->nivel += 1;
    }

    void exibir(){
        cout << "[Gerenciador]: nivel = " << this->nivel << " "
             << "ouro  = " << this->ouro << " "
             << "xp = " << this->xp << endl;
    }

    void adicionarJogador(shared_ptr<Jogador> jogador){
        if (this->jogadores.size() > 5){
            throw runtime_error("Quantidade de jogadores limite atingida!!! Total " + to_string(this->jogadores.size()) + "/5");
        }
        this->jogadores.push_back(jogador);
        cout << jogador->getNome() << " adicionado. Total de jogadores: " << this->jogadores.size() << endl;
    }

    void adicionarInimigos(shared_ptr<Inimigo> inimigo){
        this->inimigos.push_back(inimigo);
        cout << inimigo->getNome() << " adicionado. Total de inimigos: " << this->inimigos.size() << endl;
    }
};

GerenciadorDeJogo *GerenciadorDeJogo::_instancia = nullptr;

// Adiciona XP ao jogador e, se atingir o maximo, sobe de nivel (com repeticao caso suba mais de um nivel)
// Tambem registra o XP e nivel no GerenciadorDeJogo para controle global
bool Jogador::ganharXP(int experiencia){
    if (this->morto == true){
        cout << this->getNome() << " morreu e nao pode ganhar xp!\n"
             << endl;
        return false;
    }
    this->xp += experiencia;
    GerenciadorDeJogo &gerenciadorJogo = GerenciadorDeJogo::instancia();
    gerenciadorJogo.adicionarXP(experiencia);
    while (this->xp >= this->xpMax){
        this->setLevel(this->getLevel() + 1);
        this->xp -= this->xpMax;
        gerenciadorJogo.subirNivel();
    }
    cout << this->getNome() << " subiu para o nivel " << this->getLevel() << endl;
    return true;
}

map<string, string> Jogador::serializaDicionario(){ // Cria um ponto de save
    return {{"nome", this->getNome()},
            {"level", to_string(this->getLevel())},
            {"hp", to_string(this->getHP())},
            {"hpMax", to_string(this->getHPMax())},
            {"xp", to_string(this->xp)},
            // Inventario nao e serializado pois itens nao possuem reconstrucao implementada no load:
            //{"inventario": this->inventario},
            {"Raca", this->raca->nomeClasse()}, // Precisa passar cada nome de raca
            {"classe", this->nomeClasse()},     // E de classe
            {"versao", "1.0"}};
}

void Inimigo::droparRecompensas(shared_ptr<Jogador> jogador){ // Funcao que ativa caso o inimigo morra, ai ele libera a recompensas para os jogadores
    if (this->morto){
        jogador->ganharXP(this->xpRecompensa);
        GerenciadorDeJogo::instancia().adicionarOuro(this->ouroRecompensa); // Grana ganha no combate com qualquer inimigo vai para o cofrinho do time
        cout << jogador->getNome() << " ganhou " << this->xpRecompensa << " de XP e " << this->ouroRecompensa << " de grana!" << endl;
    }
}

// ------------ FACTORY: CRIACAO DE ITENS -----------------
// Padrao de projeto Factory: centraliza a criacao de armas, armaduras e pocoes
// Recebe o tipo como string e retorna o objeto correto sem expor os construtores ao resto do codigo

// Factory
class CriacaoItens{
public:
    static shared_ptr<Arma> criarArma(string tipo){
        map<string, shared_ptr<Arma>> armas = {{"espada", make_shared<Arma>("Dualiso", 200, 2, 25)},
                                               {"machado", make_shared<Arma>("Machadinha Infernal", 300, 2, 40)},
                                               {"adaga", make_shared<Arma>("Lamina Oculta", 150, 1, 20)},
                                               {"rapiera", make_shared<Arma>("Maellum", 500, 2, 35)},
                                               {"foco arcano", make_shared<Arma>("Trebuchim", 300, 1, 20)},
                                               {"foice", make_shared<Arma>("Moisson", 400, 3, 25)}};

        if (armas.count(tipo))
            return armas[tipo];
        return nullptr;
    }

    static shared_ptr<Armadura> criarArmaduras(string tipo){
        map<string, shared_ptr<Armadura>> armaduras = {{"malha", make_shared<Armadura>("Cota de Malha", 100, 2, 3)},
                                                       {"couro", make_shared<Armadura>("Armadura de Couro", 150, 2, 5)},
                                                       {"ferro", make_shared<Armadura>("Armadura de Ferro", 400, 3, 10)}};

        if (armaduras.count(tipo))
            return armaduras[tipo];
        return nullptr;
    }

    static shared_ptr<Consumiveis> criarPocao(string tipo, int quantidade = 1){
        map<string, shared_ptr<Consumiveis>> pocoes = {{"vida", make_shared<PocaoVida>(quantidade)}, // Ja tenho a variavel das pocoes, posso so cria-las agora
                                                       {"mana", make_shared<PocaoMana>(quantidade)},
                                                       {"fogo", make_shared<DanoFogo>(quantidade)},
                                                       {"gelo", make_shared<DanoGelo>(quantidade)},
                                                       {"relampago", make_shared<DanoRelampago>(quantidade)},
                                                       {"radiante", make_shared<DanoRadiante>(quantidade)},
                                                       {"venenoso", make_shared<DanoVenenoso>(quantidade)}};

        if (pocoes.count(tipo))
            return pocoes[tipo];
        return nullptr;
    }
};

// Factory para jogadores: recebe classe, nome, nivel e raca e retorna o objeto polimórfico correto
class CriacaoJogadores{
public:
    static shared_ptr<Jogador> criarJogadores(string classe, string nome, int nivel, shared_ptr<Raca> Raca){
        map<string, function<shared_ptr<Jogador>()>> jogadores = {{"bruxo", [&]()
                                                                   { return make_shared<Bruxo>(nome, nivel, Raca); }},
                                                                  {"ladrao", [&]()
                                                                   { return make_shared<Ladrao>(nome, nivel, Raca); }},
                                                                  {"artifice", [&]()
                                                                   { return make_shared<Artifice>(nome, nivel, Raca); }},
                                                                  {"guerreiro", [&]()
                                                                   { return make_shared<Guerreiro>(nome, nivel, Raca); }}};
        if (jogadores.count(classe))
            return jogadores[classe]();
        return nullptr;
    }
};

// Factory para inimigos: recebe o nome e retorna o inimigo correto via mapa de lambdas
class CriacaoInimigos{
public:
    static shared_ptr<Inimigo> criarInimigos(string nome){
        map<string, function<shared_ptr<Inimigo>()>> inimigos = {{"dragao", []()
                                                                  { return make_shared<Dragao>(); }},
                                                                 {"basilisco", []()
                                                                  { return make_shared<Basilisco>(); }},
                                                                 {"saqueadores", []()
                                                                  { return make_shared<Saqueadores>(); }}};
        if (inimigos.count(nome))
            return inimigos[nome]();
        return nullptr;
    }
};

// ----------------- IMPLEMENTACAO: ARTIFICE::CRIARITEM ------------------
// Tenta criar um item do tipo solicitado; tem 30% de chance de criar uma Pedra Inutil
// Se o item nao existir no Factory, lanca excecao
shared_ptr<Item> Artifice::criarItem(string tipo, int quantidade){
    if (this->morto == true){
        cout << this->getNome() << " esta morto e nao pode criar itens" << endl;
        return nullptr;
    }

    if (randint(0, 100) < 30){
        shared_ptr<Item> pedra = make_shared<ItemGenerico>("Pedra Inutil", 0, 1); // Ele faz uma pedra
        this->inventario.adicionarItem(pedra);                                    // E guarda a porcaria da pedra no inventario ksksksk
        cout << this->getNome() << " criou uma pedra inutil, parabens =) " << endl;
        return pedra;
    }

    shared_ptr<Item> item = CriacaoItens::criarPocao(tipo, quantidade);
    if (item == nullptr)
        item = CriacaoItens::criarArma(tipo); // Tenta criar o que o jogador solicitou (um item ou uma arma), ele nao pode criar armaduras

    if (item == nullptr){ // Se o item nao existir no jogo ou no ItemFactory
        throw runtime_error(this->getNome() + " nao sabe como criar " + tipo);
    }

    this->inventario.adicionarItem(item); // So depois de validar se o item existe, ele guarda o item criado no inventario
    cout << "Voce criou " << item->nomeItem << " com sucesso" << endl;
    return item;
}

// -------------- IMPLEMENTACAO: ATAQUES --------------------
// Cada classe tem duas versoes de atacar(): uma contra Jogador e outra contra Inimigo
// O sistema de rolagem usa d20 (dado de 20 faces): 20 = critico, 1 = falha critica, resto = normal
// O Logger registra cada ataque para exibicao ao final da partida

int Bruxo::atacar(shared_ptr<Jogador> alvo){
    cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
    if (alvo->morto){
        cout << alvo->getNome() << " ja esta morto!" << endl;
        return 0;
    }

    if (this->mana >= 10){
        this->mana -= 10;
        int roll = randint(1, 20);

        if (alvo->getArmadura() > roll){
            cout << "A armadura de " << alvo->getNome() << " absorveu todo dano" << endl;
        }

        if (roll == 20){
            alvo->receberDano(this->carisma * 2);                                                                                          // Se critar, da o dobro de dano
            Logger::instancia().log(this->getNome() + " critou e deu " + to_string(this->carisma * 2) + " de dano em " + alvo->getNome()); // Chamando a funcao "Logger" pra ja registrar no log e ja printa tambem
        }
        else if (roll == 1){
            Logger::instancia().log(this->getNome() + " errou o ataque em " + alvo->getNome()); // Se der falha critica, erra o ataque
            return 0;
        }
        else{
            alvo->receberDano(this->carisma);
            Logger::instancia().log(this->getNome() + " deu " + to_string(this->carisma) + " de dano em " + alvo->getNome());
        }

        if (alvo->morto){
            cout << alvo->getNome() << " morreu!\n"
                 << endl;
        }
        else{
            cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                 << endl;
        }
    }
    else{
        throw runtime_error(this->getNome() + " nao tem mais mana! Mana restante: " + to_string(this->mana));
    }
    return 0;
}

int Bruxo::atacar(shared_ptr<Inimigo> alvo){
    cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
    if (alvo->morto){
        cout << alvo->getNome() << " ja esta morto!" << endl;
        return 0;
    }

    if (this->mana >= 10){
        this->mana -= 10;
        int roll = randint(1, 20);

        if (alvo->getArmadura() > roll){
            cout << "A armadura de " << alvo->getNome() << " absorveu todo dano" << endl;
        }

        if (roll == 20){
            alvo->receberDano(this->carisma * 2, dynamic_pointer_cast<Jogador>(shared_from_this()));                                       // Se critar, da o dobro de dano
            Logger::instancia().log(this->getNome() + " critou e deu " + to_string(this->carisma * 2) + " de dano em " + alvo->getNome()); // Chamando a funcao "Logger" pra ja registrar no log e ja printa tambem
        }
        else if (roll == 1){
            Logger::instancia().log(this->getNome() + " errou o ataque em " + alvo->getNome()); // Se der falha critica, erra o ataque
            return 0;
        }
        else{
            alvo->receberDano(this->carisma, dynamic_pointer_cast<Jogador>(shared_from_this()));
            Logger::instancia().log(this->getNome() + " deu " + to_string(this->carisma) + " de dano em " + alvo->getNome());
        }

        if (alvo->morto){
            cout << alvo->getNome() << " morreu!\n"
                 << endl;
        }
        else{
            cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                 << endl;
        }
    }
    else{
        throw runtime_error(this->getNome() + " nao tem mais mana! Mana restante: " + to_string(this->mana));
    }
    return 0;
}

int Ladrao::atacar(shared_ptr<Jogador> alvo){
    cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
    if (alvo->morto){
        cout << alvo->getNome() << " ja esta morto!" << endl;
        return 0;
    }

    if (this->armaEquipada != nullptr){
        int roll = randint(1, 20);
        if (alvo->getArmadura() > roll){
            cout << "A armadura de " << alvo->getNome() << " absorveu todo dano" << endl;
        }

        if (roll == 20){
            alvo->receberDano(this->dano * 4); // Se critar, da o 4x mais dano
            // Sucesso critico registrado no log abaixo
            Logger::instancia().log("SUCESSO CRITICO!!! " + this->getNome() + " deu " + to_string(this->agilidade * 4) + " de dano em " + alvo->getNome());
        }

        else if (roll == 1){
            Logger::instancia().log(this->getNome() + " errou o ataque em " + alvo->getNome());
            return 0;
        }
        else if (roll > 12){
            alvo->receberDano(this->agilidade * 2);
            Logger::instancia().log(this->getNome() + " critou e deu " + to_string(this->dano * 2) + " de dano em " + alvo->getNome());
        }
        else{
            alvo->receberDano(this->agilidade);
            Logger::instancia().log(this->getNome() + " deu " + to_string(this->dano) + " de dano em " + alvo->getNome());
        }

        if (alvo->morto){
            cout << alvo->getNome() << " morreu!\n"
                 << endl;
        }
        else{
            cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                 << endl;
        }
    }
    else
    {
        cout << this->getNome() << " estava sem arma equipada para atacar" << endl;
        alvo->receberDano(10);
        Logger::instancia().log(this->getNome() + " deu um tapa em " + alvo->getNome());
        return 10; // Dano rancado no tapa
    }
    return 0;
}


    int Ladrao::atacar(shared_ptr<Inimigo> alvo)
    {
        cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
        if (alvo->morto)
        {
            cout << alvo->getNome() << " ja esta morto!" << endl;
            return 0;
        }

        if (this->armaEquipada != nullptr)
        {
            int roll = randint(1, 20);
            if (alvo->getArmadura() > roll)
            {
                cout << "A armadura de " << alvo->getNome() << " absorveu todo dano" << endl;
            }

            if (roll == 20)
            {
                alvo->receberDano(this->dano * 4, dynamic_pointer_cast<Jogador>(shared_from_this())); // Se critar, da o 4x mais dano
                // Sucesso critico registrado no log abaixo
                Logger::instancia().log("SUCESSO CRITICO!!! " + this->getNome() + " deu " + to_string(this->agilidade * 4) + " de dano em " + alvo->getNome());
            }

            else if (roll == 1)
            {
                Logger::instancia().log(this->getNome() + " errou o ataque em " + alvo->getNome());
                return 0;
            }
            else if (roll > 12)
            {
                alvo->receberDano(this->agilidade * 2, dynamic_pointer_cast<Jogador>(shared_from_this()));
                Logger::instancia().log(this->getNome() + " critou e deu " + to_string(this->dano * 2) + " de dano em " + alvo->getNome());
            }
            else{
            alvo->receberDano(this->agilidade, dynamic_pointer_cast<Jogador>(shared_from_this()));
            Logger::instancia().log(this->getNome() + " deu " + to_string(this->dano) + " de dano em " + alvo->getNome());
        }

        if (alvo->morto){
            cout << alvo->getNome() << " morreu!\n"
                 << endl;
        }
        else{
            cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                 << endl;
        }
    }
    else
    {
        cout << this->getNome() << " estava sem arma equipada para atacar" << endl;
        alvo->receberDano(10, dynamic_pointer_cast<Jogador>(shared_from_this()));
        Logger::instancia().log(this->getNome() + " deu um tapa em " + alvo->getNome());
        return 10; // Dano rancado no tapa
    }
    return 0;
    }


        int Artifice::atacar(shared_ptr<Jogador> alvo)
        {
            cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
            if (alvo->morto)
            {
                cout << alvo->getNome() << " ja esta morto!" << endl;
                return 0;
            }

            if (this->armaEquipada != nullptr)
            {
                if (this->mana >= 10)
                {
                    this->mana -= 10;
                    int roll = randint(1, 20);

                    if (alvo->getArmadura() > roll)
                    {
                        cout << "A armadura de " << alvo->getNome() << " absorveu todo dano" << endl;
                    }

                    if (roll == 20)
                    {
                        alvo->receberDano(this->inteligencia * 2); // Se critar, da o dobro de dano
                        Logger::instancia().log(this->getNome() + " critou e deu " + to_string(this->inteligencia * 2) + " de dano em " + alvo->getNome());
                    }

                    else if (roll == 1)
                    {
                        Logger::instancia().log(this->getNome() + " errou o ataque em " + alvo->getNome());
                    }
                    else
                    {
                        alvo->receberDano(this->inteligencia);
                        Logger::instancia().log(this->getNome() + " deu " + to_string(this->inteligencia) + " de dano em " + alvo->getNome());
                    }

                    if (alvo->morto)
                    {
                        cout << alvo->getNome() << " morreu!\n"
                             << endl;
                    }
                    else
                    {
                        cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                             << endl;
                    }
                }
                else
                {
                    cout << this->getNome() << " nao tem mais mana! Mana restante: " << this->mana << endl;
                }
            }
            else{
        try{
            shared_ptr<Item> pedra = this->inventario.buscarItens("Pedra Inutil");
            pedra->quantidade -= 1;
            if (pedra->quantidade < 1){
                this->inventario.removerItem("Pedra Inutil"); // Se acabou as pedras do inventario, remove e libera espaco
            }
            alvo->receberDano(10);
            Logger::instancia().log(this->getNome() + " tacou uma pedra em " + alvo->getNome());
        }
        catch (...){
            alvo->receberDano(5);
            Logger::instancia().log(this->getNome() + " deu um tapa em " + alvo->getNome());
            return 5; // Dano rancado no tapa
        }
    }
    return 0;
}


    int Artifice::atacar(shared_ptr<Inimigo> alvo){
        cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
            if (alvo->morto){
                cout << alvo->getNome() << " ja esta morto!" << endl;
                return 0;
            }

            if (this->armaEquipada != nullptr){
                if (this->mana >= 10){
                    this->mana -= 10;
                    int roll = randint(1, 20);

                    if (alvo->getArmadura() > roll){
                        cout << "A armadura de " << alvo->getNome() << " absorveu todo dano" << endl;
                    }

                    if (roll == 20){
                        alvo->receberDano(this->inteligencia * 2, dynamic_pointer_cast<Jogador>(shared_from_this())); // Se critar, da o dobro de dano
                        Logger::instancia().log(this->getNome() + " critou e deu " + to_string(this->inteligencia * 2) + " de dano em " + alvo->getNome());
                    }

                    else if (roll == 1){
                        Logger::instancia().log(this->getNome() + " errou o ataque em " + alvo->getNome());
                    }
                    else{
                        alvo->receberDano(this->inteligencia, dynamic_pointer_cast<Jogador>(shared_from_this()));
                        Logger::instancia().log(this->getNome() + " deu " + to_string(this->inteligencia) + " de dano em " + alvo->getNome());
                    }

                    if (alvo->morto){
                        cout << alvo->getNome() << " morreu!\n"
                             << endl;
                    }
                    else{
                        cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                             << endl;
                    }
                }
                else
                {
                    cout << this->getNome() << " nao tem mais mana! Mana restante: " << this->mana << endl;
                }
            }
            else{
                try{
                    shared_ptr<Item> pedra = this->inventario.buscarItens("Pedra Inutil");
                    pedra->quantidade -= 1;
                    if (pedra->quantidade < 1)
                    {
                        this->inventario.removerItem("Pedra Inutil"); // Se acabou as pedras do inventario, remove e libera espaco
                    }
                    alvo->receberDano(10, dynamic_pointer_cast<Jogador>(shared_from_this()));
                    Logger::instancia().log(this->getNome() + " tacou uma pedra em " + alvo->getNome());
                }
                catch (...){
                    alvo->receberDano(5, dynamic_pointer_cast<Jogador>(shared_from_this()));
                    Logger::instancia().log(this->getNome() + " deu um tapa em " + alvo->getNome());
                    return 5; // Dano rancado no tapa
                }
            }
            return 0;
        }

        int Guerreiro::atacar(shared_ptr<Jogador> alvo){
            cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
            if (alvo->morto){
                cout << alvo->getNome() << " ja esta morto!" << endl;
                return 0;
            }

            if (this->armaEquipada != nullptr){ // Se a arma nao tiver equipada, se fode e da o dano default
                int roll = randint(1, 20);
                if (alvo->getArmadura() > roll){
                    cout << "A armadura de " << alvo->getNome() << " absorveu todo dano" << endl;
                }

                if (roll == 20){
                    alvo->receberDano(this->forca * 2);
                    cout << "SUCESSO CRITICO" << endl;
                    Logger::instancia().log(this->getNome() + " deu " + to_string(this->forca * 2) + " de dano em " + alvo->getNome());
                }

                else if (roll == 1){
                    Logger::instancia().log(this->getNome() + " errou o ataque em " + alvo->getNome());
                    return 0;
                }

                else{
                    alvo->receberDano(this->forca);
                    Logger::instancia().log(this->getNome() + " deu " + to_string(this->forca) + " de dano em " + alvo->getNome());
                }

                if (alvo->morto){
                    cout << alvo->getNome() << " morreu!\n"
                         << endl;
                }
                else{
                    cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                         << endl;
                }
            }
            else{
                cout << this->getNome() << " estava sem arma equipada para atacar" << endl;
                alvo->receberDano(20);
                Logger::instancia().log(this->getNome() + " deu um murro em " + alvo->getNome());
                return 20; // Dano rancado no murro
            }
            return 0;
        }

        int Guerreiro::atacar(shared_ptr<Inimigo> alvo){
            cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
            if (alvo->morto){
                cout << alvo->getNome() << " ja esta morto!" << endl;
                return 0;
            }

            if (this->armaEquipada != nullptr){ // Se a arma nao tiver equipada, se fode e da o dano default
                int roll = randint(1, 20);
                if (alvo->getArmadura() > roll){
                    cout << "A armadura de " << alvo->getNome() << " absorveu todo dano" << endl;
                }

                if (roll == 20){
                    alvo->receberDano(this->forca * 2, dynamic_pointer_cast<Jogador>(shared_from_this()));
                    cout << "SUCESSO CRITICO" << endl;
                    Logger::instancia().log(this->getNome() + " deu " + to_string(this->forca * 2) + " de dano em " + alvo->getNome());
                }

                else if (roll == 1){
                    Logger::instancia().log(this->getNome() + " errou o ataque em " + alvo->getNome());
                    return 0;
                }

                else{
                    alvo->receberDano(this->forca, dynamic_pointer_cast<Jogador>(shared_from_this()));
                    Logger::instancia().log(this->getNome() + " deu " + to_string(this->forca) + " de dano em " + alvo->getNome());
                }

                if (alvo->morto){
                    cout << alvo->getNome() << " morreu!\n"
                         << endl;
                }
                else{
                    cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                         << endl;
                }
            }
            else{
                cout << this->getNome() << " estava sem arma equipada para atacar" << endl;
                alvo->receberDano(20, dynamic_pointer_cast<Jogador>(shared_from_this()));
                Logger::instancia().log(this->getNome() + " deu um murro em " + alvo->getNome());
                return 20; // Dano rancado no murro
            }
            return 0;
        }

        int Dragao::atacar(shared_ptr<Jogador> alvo){
            cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
            if (alvo->morto){
                cout << alvo->getNome() << " ja esta morto!" << endl;
                return 0;
            }
            int roll = randint(1, 20);
            if (roll == 1){
                Logger::instancia().log(this->getNome() + " falhou miseravelmente no seu ataque");
                return 0;
            }
            alvo->receberDano(this->dano);
            Logger::instancia().log(this->getNome() + " soltou o bafo do selvagem no " + alvo->getNome() + " e deu " + to_string(this->dano) + " de dano");
            if (alvo->morto){
                cout << alvo->getNome() << " virou carvao!\n"
                     << endl;
            }
            else{
                cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                     << endl;
            }
            return 0;
        }

        int Basilisco::atacar(shared_ptr<Jogador> alvo){
            cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
            if (alvo->morto){
                cout << alvo->getNome() << " ja esta morto!" << endl;
                return 0;
            }
            int roll = randint(1, 20);
            if (roll == 1){
                Logger::instancia().log(this->getNome() + " falhou miseravelmente no seu ataque");
                return 0;
            }
            alvo->receberDano(this->dano);
            Logger::instancia().log(this->getNome() + " mordeu " + alvo->getNome() + " e deu " + to_string(this->dano) + " de dano");
            if (alvo->morto){
                cout << alvo->getNome() << " foi engolido pelo " << this->getNome() << "!\n"
                     << endl;
            }
            else{
                cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                     << endl;
            }
            return 0;
        }

        int Saqueadores::atacar(shared_ptr<Jogador> alvo){
            cout << this->getNome() << " vai atacar " << alvo->getNome() << ":" << endl;
            if (alvo->morto){
                cout << alvo->getNome() << " ja esta morto!" << endl;
                return 0;
            }
            int roll = randint(1, 20);
            if (roll == 1){
                Logger::instancia().log(this->getNome() + " falhou miseravelmente no seu ataque");
                return 0;
            }
            alvo->receberDano(this->dano * 2); // Sao dois saqueadores que atacam ao mesmo tempo
            Logger::instancia().log(this->getNome() + " esfaquearam o bucho do " + alvo->getNome() + " e deram " + to_string(this->dano * 2) + " de dano");
            if (alvo->morto){
                cout << alvo->getNome() << " virou estatistica!\n"
                     << endl;
            }
            else{
                cout << alvo->getNome() << ": " << alvo->getHP() << "/" << alvo->getHPMax() << "\n"
                     << endl;
            }
            return 0;
        }

        // -------------- CLASSE MASMORRA ----------------
        // Gerencia o combate em grupo: lista inimigos disponiveis, recebe a escolha do jogador
        // e conduz os turnos alternados (jogadores agem, depois o inimigo ataca um alvo aleatorio)
        // Jogadores podem atacar, usar itens ou tentar fugir a cada turno
        class Masmorra{
        public:
            map<string, shared_ptr<Inimigo>> inimigosDisponiveis;

            Masmorra(){
                this->inimigosDisponiveis = {{"1", CriacaoInimigos::criarInimigos("saqueadores")},
                                             {"2", CriacaoInimigos::criarInimigos("basilisco")},
                                             {"3", CriacaoInimigos::criarInimigos("dragao")}};
            }

            void entrar(){
                GerenciadorDeJogo &gerenciadorJogo = GerenciadorDeJogo::instancia(); // Instanciou o gerenciador de jogo
                vector<shared_ptr<Jogador>> jogadoresVivos;
                for (auto j : gerenciadorJogo.jogadores){ // Laco pra ver se tem alguem vivo e quantos tem
                    if (j->morto == false){
                        jogadoresVivos.push_back(j);
                    }
                }

                if (jogadoresVivos.empty()){ // Se nao tiver, so sai da masmorra
                    cout << "Ta fazendo o que aqui? Ta todo mundo morto paizao!" << endl;
                    return;
                }

                cout << "Bem vindo a masmorra" << endl;
                cout << "Escolha um inimigo para enfrentar: " << endl;
                for (auto par : this->inimigosDisponiveis)
                { // Faz o dicionario do game retornar a key e o inimigo do bloco de inimigosDisponiveis
                    cout << par.first << ". " << par.second->getNome() << " // HP: " << par.second->getHP() << " // Dano: " << par.second->dano << endl;
                }
                string escolha;
                cout << "Escolha: ";
                cin >> escolha;

                if (this->inimigosDisponiveis.count(escolha) == 0){ // Se o numero da key escolhida nao existir, ele da erro e retorna (nao pode ser diferente, pois ele ta comparando uma string com o gerenciador de jogo)
                    cout << "Burroooo! Opcao invalida!" << endl;
                    return;
                }

                shared_ptr<Inimigo> inimigo = this->inimigosDisponiveis[escolha]; // Salva sua escolha em inimigo
                this->combate(inimigo);
            }

            void combate(shared_ptr<Inimigo> inimigo){
                GerenciadorDeJogo &gerenciadorJogo = GerenciadorDeJogo::instancia();
                vector<shared_ptr<Jogador>> jogadores;
                vector<shared_ptr<Jogador>> fugitivos;
                for (auto j : gerenciadorJogo.jogadores){ // Laco pra ver se tem alguem vivo e quantos tem
                    if (j->morto == false){
                        jogadores.push_back(j);
                    }
                }

                cout << "Turminha do barulho vs " << inimigo->getNome() << endl;

                while (inimigo->morto == false){
                    vector<shared_ptr<Jogador>> jogadoresVivos;
                    for (auto j : jogadores){ // Laco pra ver se tem alguem vivo e quantos tem
                        if (j->morto == false){
                            if (find(fugitivos.begin(), fugitivos.end(), j) == fugitivos.end()){
                                jogadoresVivos.push_back(j);
                            }
                        }
                    }
                    if (jogadoresVivos.empty()){ // Tem que ver se a lista esta vazia
                        cout << "\nO grupo todo morreu! GAME OVER!\n"
                             << endl;
                        return;
                    }

                    for (auto jogador : jogadoresVivos){
                        cout << "Turno do " << jogador->getNome() << " // HP: " << jogador->getHP() << endl;
                        cout << "1. Atacar" << endl;
                        cout << "2. Curar" << endl;
                        cout << "3. Recuperar Mana" << endl;
                        cout << "4. Tacar pocao de dano" << endl;
                        cout << "5. Ressucitar alguem" << endl;
                        cout << "6. Fugir" << endl;

                        string acao;
                        cout << "Acao: ";
                        cin >> acao;

                        if (acao == "1"){
                            try{
                                jogador->atacar(inimigo);
                            }
                            catch (runtime_error &){
                                cout << "Acabou a mana ;-; !" << endl;
                            }
                        }
                        else if (acao == "2"){
                            try{
                                shared_ptr<Consumiveis> pocao = dynamic_pointer_cast<Consumiveis>(jogador->inventario.buscarItens("Pocao de Vida"));
                                pocao->usarItem(jogador, jogador);
                            }
                            catch (runtime_error &){
                                cout << "Acabou as pocoes ;-; !" << endl;
                            }
                        }
                        else if (acao == "3"){
                            try{
                                shared_ptr<Consumiveis> pocao = dynamic_pointer_cast<Consumiveis>(jogador->inventario.buscarItens("Pocao de Mana"));
                                pocao->usarItem(jogador);
                            }
                            catch (runtime_error &){
                                cout << "Acabou as pocoes ;-; !" << endl;
                            }
                        }
                        else if (acao == "4"){
                            cout << "Qual pocao deseja tacar: " << endl;
                            cout << "1. Dano de Fogo" << endl;
                            cout << "2. Dano de Gelo" << endl;
                            cout << "3. Dano por Relampago" << endl;
                            cout << "4. Dano Radiante" << endl;
                            cout << "5. Dano Venenoso" << endl;

                            string efeito;
                            cout << "Efeito: ";
                            cin >> efeito;

                            if (efeito == "1"){
                                try{
                                    shared_ptr<Consumiveis> pocao = dynamic_pointer_cast<Consumiveis>(jogador->inventario.buscarItens("Pocao de Fogo"));
                                    pocao->usarItem(jogador, inimigo);
                                }
                                catch (runtime_error &){
                                    cout << "Acabou as pocoes ;-; !" << endl;
                                }
                            }
                            else if (efeito == "2"){
                                try{
                                    shared_ptr<Consumiveis> pocao = dynamic_pointer_cast<Consumiveis>(jogador->inventario.buscarItens("Pocao de Gelo"));
                                    pocao->usarItem(jogador, inimigo);
                                }
                                catch (runtime_error &){
                                    cout << "Acabou as pocoes ;-; !" << endl;
                                }
                            }
                            else if (efeito == "3"){
                                try{
                                    shared_ptr<Consumiveis> pocao = dynamic_pointer_cast<Consumiveis>(jogador->inventario.buscarItens("Pocao de Relampago"));
                                    pocao->usarItem(jogador, inimigo);
                                }
                                catch (runtime_error &){
                                    cout << "Acabou as pocoes ;-; !" << endl;
                                }
                            }
                            else if (efeito == "4"){
                                try{
                                    shared_ptr<Consumiveis> pocao = dynamic_pointer_cast<Consumiveis>(jogador->inventario.buscarItens("Pocao Radiante"));
                                    pocao->usarItem(jogador, inimigo);
                                }
                                catch (runtime_error &){
                                    cout << "Acabou as pocoes ;-; !" << endl;
                                }
                            }
                            else if (efeito == "5"){
                                try{
                                    shared_ptr<Consumiveis> pocao = dynamic_pointer_cast<Consumiveis>(jogador->inventario.buscarItens("Pocao Venenosa"));
                                    pocao->usarItem(jogador, inimigo);
                                }
                                catch (runtime_error &){
                                    cout << "Acabou as pocoes ;-; !" << endl;
                                }
                            }
                            else{
                                throw runtime_error("Burroooo! Opcao invalida");
                            }
                        }
                        else if (acao == "5"){
                            try{
                                shared_ptr<Consumiveis> pocao = dynamic_pointer_cast<Consumiveis>(jogador->inventario.buscarItens("Orbe da Ressureicao"));
                                pocao->usarItem(jogador, jogador);
                            }
                            catch (runtime_error &){
                                cout << "Acabou o orbe ;-; !" << endl;
                            }
                        }
                        else if (acao == "6"){
                            int roll = randint(1, 20);
                            if (jogador->classe == "Ladrao"){
                                roll += jogador->furtividade;
                            }
                            if (roll > 15){
                                cout << jogador->getNome() << " fugiu com sucesso!" << endl;
                                fugitivos.push_back(jogador);                                                                              // Lista de fugitivos
                                jogadoresVivos.erase(remove(jogadoresVivos.begin(), jogadoresVivos.end(), jogador), jogadoresVivos.end()); // Precisa remover das duas listasa, se nao da bosta
                                if (jogadoresVivos.size() == 0)
                                {
                                    cout << "O grupo todo conseguiu fugir!" << endl;
                                    return;
                                }
                            }
                            else{
                                cout << "HOJE NAO!" << endl;
                            }
                        }

                        if (inimigo->morto){
                            break;
                        }
                    }

                    if (inimigo->morto == false){ // Atualiza pra ver se alguem morreu depois da rodada do inimigo
                        jogadoresVivos = {};
                        for (auto j : jogadores){ // Laco pra ver se tem alguem vivo e quantos tem
                            if (j->morto == false){
                                jogadoresVivos.push_back(j); // Junta todos os herois vivos
                            }
                        }
                        if (!jogadoresVivos.empty()){
                            shared_ptr<Jogador> alvo = choice(jogadoresVivos); // Escolhe aleatoriamente o candango que vai ser atacado
                            inimigo->atacar(alvo);
                        }
                    }
                }

                if (inimigo->morto){                                                          // Se matar o inimigo
                    gerenciadorJogo.estado[inimigo->estadoMonstro] = true; // Muda o estado do bichano que o grupo batalhou para morto (usa polimorfismo pra pegar o bichano certo)
                    cout << "O grupo de Herois venceu" << endl;
                    for (auto jogador : jogadores){
                        if (!jogador->morto){
                            jogador->ganharXP(inimigo->xpRecompensa); // Todo mundo ganha xp e quem matou ganha de novo como bonus por ter matado
                        }
                    }
                    gerenciadorJogo.exibir();
                }
            }
        };

        // ---------- CLASSE MERCADAO -----------
        // Permite ao grupo vender e comprar itens; interage com o Singleton GerenciadorDeJogo
        // para adicionar ou descontar ouro do cofrinho compartilhado e atualizar contadores de missao
        class Mercadao{
        public:
            void venderEspolio(shared_ptr<Item> item){
                GerenciadorDeJogo &gerenciadorJogo = GerenciadorDeJogo::instancia(); // Chama o singleton (gerenciador de jogo)
                gerenciadorJogo.adicionarOuro(item->valor);                          // E vende o item
                gerenciadorJogo.estado["itensVendidos"] += 1;
                cout << "[Mercadao]: " << item->str() << " foi barganhado!" << endl;
                gerenciadorJogo.exibir();
            }

            void comprarEspolio(shared_ptr<Item> item){
                GerenciadorDeJogo &gerenciadorJogo = GerenciadorDeJogo::instancia();
                gerenciadorJogo.comprarItem(item); // Tenta comprar o item se tiver dinheiro
                gerenciadorJogo.estado["itensComprados"] += 1;
                cout << "[Mercadao]: " << item->str() << " foi barganhado!" << endl;
                gerenciadorJogo.exibir();
            }
        };

        // ---------- CLASSES MISSAO E QUADRO DE MISSOES ------------
        // Missao armazena nome, descricao, recompensas e uma funcao lambda que verifica se a condicao foi atendida
        // QuadroMissoes agrupa todas as missoes disponiveis e verifica/distribui recompensas

        class Missao{ // Classe pra validar se tal missao foi feita e dar as recompensas
        public:
            string nome;
            string descricao;
            int recompensaXP;
            int recompensaOuro;
            function<bool(map<string, int>)> condicao;
            bool concluida;
            bool recompensaRetirada;

            Missao(string nome, string descricao, int recompensaXP, int recompensaOuro, function<bool(map<string, int>)> condicao){
                this->nome = nome;
                this->descricao = descricao;
                this->recompensaXP = recompensaXP;
                this->recompensaOuro = recompensaOuro;
                this->condicao = condicao;
                this->concluida = false;
                this->recompensaRetirada = false;
            }
        };

        // Algumas missoes
        class QuadroMissoes{
        public:
            vector<Missao> missoes;

            QuadroMissoes(){
                this->missoes = {Missao("Cacador de Dragoes", "Derrote um dragao", 200, 150, [](map<string, int> estado)
                                        { return estado["dragaoMorto"]; }),
                                 Missao("Exterminador de Feras", "Derrote um basilisco", 150, 100, [](map<string, int> estado)
                                        { return estado["basiliscoMorto"]; }),
                                 Missao("Muambeiro", "Venda 5 item na loja", 30, 20, [](map<string, int> estado)
                                        { return estado["itensVendidos"] >= 5; }),
                                 Missao("Meu Dinheirinho", "Compre 5 item da loja", 50, 30, [](map<string, int> estado)
                                        { return estado["itensComprados"] >= 5; })};
            }

            void verificarMissoes(){
                GerenciadorDeJogo &gerenciadorJogo = GerenciadorDeJogo::instancia();
                for (auto &missao : this->missoes){ // Faz um loop para ir verificando se alguma missao do quadro de missoes
                    if (missao.concluida == false){ // Se a missao consta como nao concluida
                        if (missao.condicao(gerenciadorJogo.estado)){                            // Mas ela foi concluida
                            missao.concluida = true; // Troca o estado dela para concluida
                            cout << "Missao '" << missao.nome << "' concluida! Retire sua recompensa :>" << endl;
                        }
                    }
                }
            }

            bool retirarRecompensa(string nomeMissao, shared_ptr<Jogador> jogador){
                for (auto &missao : this->missoes){
                    if (missao.nome == nomeMissao){
                        if (missao.concluida == false){
                            cout << "Missao '" << nomeMissao << "' ainda nao concluida! " << endl;
                            return false;
                        }
                        if (missao.recompensaRetirada){
                            cout << "Recompensa ja foi retirada, circulando!" << endl;
                            return false;
                        }
                        jogador->ganharXP(missao.recompensaXP);
                        GerenciadorDeJogo::instancia().adicionarOuro(missao.recompensaOuro); // Guardando a grana no cofrinho compartilhado
                        missao.recompensaRetirada = true;
                        cout << "Recompensa retirada! Voce ganhou " << missao.recompensaXP << " de XP e " << missao.recompensaOuro << " de moedas de ouro" << endl;
                        return true;
                    }
                }
                return false;
            }
        };

        // ------------- CLASSE SISTEMAS DE ARQUIVOS (SAVE/LOAD) -------------------
        // salvarJogo() serializa o estado do jogador em um arquivo JSON simples
        // carregarJogo() le o JSON, reconstroi o mapa de dados e usa o Factory para recriar o objeto do jogador
        class SistemasArquivos{
        public:
            static void salvarJogo(shared_ptr<Jogador> jogador, string arquivo = "save.json"){ // Salvando o jogo em um json
                map<string, string> dados = jogador->serializaDicionario();
                ofstream f(arquivo); // Abre o arquivo em modo de escrita ("write") e salva em f
                f << "{\n";
                int contador = 0;
                for (auto par : dados){
                    f << "  \"" << par.first << "\": \"" << par.second << "\"";
                    contador++;
                    if (contador < (int)dados.size())
                        f << ",";
                    f << "\n";
                }
                f << "}\n"; // Salva os dados em f e identa com 2 espacos
                f.close();
                cout << "Jogo salvo em " << arquivo << endl;
            }

            static shared_ptr<Jogador> carregarJogo(string arquivo = "save.json"){
                ifstream teste(arquivo);
                if (!teste.good()){ // Se o arquivo "arquivo" nao existir
                    cout << "Arquivo " << arquivo << " nao encontrado!" << endl;
                    return nullptr;
                }
                teste.close();

                ifstream f(arquivo); // Carrega o arquivo em modo leitura ("read") de f
                string conteudo((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
                f.close();

                map<string, string> dados;
                regex padrao("\\\"([^\\\"]+)\\\"\\s*:\\s*\\\"([^\\\"]*)\\\"");
                auto inicio = sregex_iterator(conteudo.begin(), conteudo.end(), padrao);
                auto fim = sregex_iterator();
                for (auto it = inicio; it != fim; ++it){
                    smatch match = *it;
                    dados[match[1]] = match[2]; // Carrega o json do arquivo f
                }

                string classe = dados["classe"];
                shared_ptr<Jogador> jogador = Jogador::desserializaDicionario(dados); // usa a reconstrucao correta (raca certa)
                if (jogador != nullptr && dados.count("hpMax")){
                    jogador->setHPMax(stoi(dados["hpMax"])); // restaura o hpMax, que nao vem pelo construtor
                }
                cout << "Jogo carregado de " << arquivo << endl;
                return jogador;
            }
        };

        shared_ptr<Jogador> Jogador::desserializaDicionario(const map<string, string> &dados){ // Volta no save (reconstroi o objeto do mesmo jeito que foi construido no ponto de save)
            map<string, shared_ptr<Raca>> Racas = {{"MeioElfo", make_shared<MeioElfo>()},
                                                   {"Humano", make_shared<Humano>()},
                                                   {"Tiefling", make_shared<Tiefling>()}};
            shared_ptr<Raca> racaObj = Racas.count(dados.at("Raca")) ? Racas[dados.at("Raca")] : make_shared<Humano>(); // Reconstroi o objeto de Raca pelo nome

            string classe = dados.at("classe");
            shared_ptr<Jogador> j = nullptr;
            if (classe == "Bruxo")
                j = make_shared<Bruxo>(dados.at("nome"), stoi(dados.at("level")), racaObj, stoi(dados.at("hp")), stoi(dados.at("xp"))); // Polimorfismo
            else if (classe == "Ladrao")
                j = make_shared<Ladrao>(dados.at("nome"), stoi(dados.at("level")), racaObj, stoi(dados.at("hp")), stoi(dados.at("xp")));
            else if (classe == "Artifice")
                j = make_shared<Artifice>(dados.at("nome"), stoi(dados.at("level")), racaObj, stoi(dados.at("hp")), stoi(dados.at("xp")));
            else if (classe == "Guerreiro")
                j = make_shared<Guerreiro>(dados.at("nome"), stoi(dados.at("level")), racaObj, stoi(dados.at("hp")), stoi(dados.at("xp")));

            return j;
        }

        // Sobrecarga do operador << para exibir jogadores diretamente com cout
        // Se o ponteiro for nulo, exibe "None"
        ostream &operator<<(ostream &os, const shared_ptr<Jogador> &jogador){
            if (jogador != nullptr)
                os << jogador->str();
            else
                os << "None";
            return os;
        }

        // --------------- MAIN ----------------
        // Ponto de entrada do programa; demonstra todas as funcionalidades implementadas:
        // criacao de jogadores e inimigos via Factory, uso de itens, combate, Singleton,
        // quadro de missoes e sistema de save/load

        int main(){
            srand(time(nullptr));

            // Criando jogadores pelo Factory
            shared_ptr<Jogador> jogador1 = CriacaoJogadores::criarJogadores("bruxo", "Lucien", 5, make_shared<MeioElfo>());
            shared_ptr<Jogador> jogador2 = CriacaoJogadores::criarJogadores("ladrao", "Sevras", 5, make_shared<MeioElfo>());
            shared_ptr<Jogador> jogador3 = CriacaoJogadores::criarJogadores("artifice", "Jimothy", 5, make_shared<Tiefling>());
            shared_ptr<Jogador> jogador4 = CriacaoJogadores::criarJogadores("guerreiro", "Tsarin", 5, make_shared<Humano>());

            // Adicionando jogadores no Singleton (GerenciadorDeJogo) para o grupo poder entrar na masmorra junto
            GerenciadorDeJogo &gerenciadorJogo = GerenciadorDeJogo::instancia();
            gerenciadorJogo.adicionarJogador(jogador1);
            gerenciadorJogo.adicionarJogador(jogador2);
            gerenciadorJogo.adicionarJogador(jogador3);
            gerenciadorJogo.adicionarJogador(jogador4);

            // Criando itens pelo Factory
            shared_ptr<Consumiveis> pocaoVida = CriacaoItens::criarPocao("vida", 3);
            shared_ptr<Consumiveis> pocaoMana = CriacaoItens::criarPocao("mana", 2);
            shared_ptr<Consumiveis> fogoItem = CriacaoItens::criarPocao("fogo", 2);
            shared_ptr<Consumiveis> geloItem = CriacaoItens::criarPocao("gelo", 1);
            shared_ptr<Consumiveis> orbeItem = make_shared<OrbeRessureicao>(1);
            shared_ptr<Arma> espada = CriacaoItens::criarArma("espada");
            shared_ptr<Arma> foice = CriacaoItens::criarArma("foice");
            shared_ptr<Armadura> armaduraFerro = CriacaoItens::criarArmaduras("ferro");

            // Adicionando itens no inventario de cada jogador
            jogador1->inventario.adicionarItem(pocaoVida); // Bruxo com pocoes de vida, mana e fogo
            jogador1->inventario.adicionarItem(pocaoMana);
            jogador1->inventario.adicionarItem(fogoItem);
            jogador2->inventario.adicionarItem(espada);        // Ladrao com espada
            jogador3->inventario.adicionarItem(orbeItem);      // Artifice com orbe de ressureicao
            jogador4->inventario.adicionarItem(armaduraFerro); // Guerreiro com armadura de ferro

            cout << "\n--- INVENTARIO DO JOGADOR 1 ---" << endl;
            jogador1->inventario.listarItens();

            // Equipando arma e armadura
            cout << "\n--- EQUIPANDO ---" << endl;
            espada->equipar(jogador2);        // Ladrao equipa a espada
            armaduraFerro->equipar(jogador4); // Guerreiro equipa a armadura

            // Testando heranca com isinstance (compara jogador com Bruxo, pra saber se )
            cout << "\n--- VERIFICANDO HERANCA ---" << endl;
            cout << "jogador1 e Bruxo? " << (dynamic_pointer_cast<Bruxo>(jogador1) != nullptr) << endl;   // Compara jogador com a classe Bruxo, pra saber se ele e realmente da classe Bruxo (e como ele é, vai retornar True)
            cout << "jogador1 e Jogador? " << (jogador1 != nullptr) << endl;                              // Compara jogador com a classe Jogador e como Bruxo herda de Jogador (ai o polimorfismo), retorna True tambem
            cout << "jogador1 e Ladrao? " << (dynamic_pointer_cast<Ladrao>(jogador1) != nullptr) << endl; // Compara jogador com  a classe Ladrao, como ele nao é, retorna "False"

            // Status de todos os jogadores
            cout << "\n--- STATUS INICIAL ---" << endl;
            cout << jogador1 << endl;
            cout << jogador2 << endl;
            cout << jogador3 << endl;
            cout << jogador4 << endl;

            // Combate entre jogadores (testando polimorfismo do atacar)
            cout << "\n--- COMBATE ENTRE JOGADORES ---" << endl;
            jogador1->atacar(jogador2); // Bruxo ataca Ladrao com magia
            jogador2->atacar(jogador1); // Ladrao ataca Bruxo com espada equipada
            jogador3->atacar(jogador1); // Artifice ataca Bruxo (sem arma, tenta usar pedra ou tapa)
            jogador4->atacar(jogador1); // Guerreiro ataca Bruxo com forca

            // Usando itens (testando polimorfismo do usarItem)
            cout << "\n--- USANDO ITENS ---" << endl;
            try{
                pocaoVida->usarItem(jogador1, jogador1); // Bruxo se cura
                pocaoMana->usarItem(jogador1);           // Bruxo recupera mana
                fogoItem->usarItem(jogador1, jogador2);  // Bruxo usa pocao de fogo no Ladrao (usuario - > Bruxo, alvo - > Ladrao)
                orbeItem->usarItem(jogador3, jogador1);  // Artifice tenta ressucitar o Bruxo com o orbe
            }
            catch (runtime_error &e){
                cout << e.what() << endl; // Se nao tiver pocao ou orbe, da erro
            }

            // Artifice criando item e atacando com pedra
            cout << "\n--- ARTIFICE CRIANDO ITEM ---" << endl;
            dynamic_pointer_cast<Artifice>(jogador3)->criarItem("vida", 2); // Artifice tenta criar 2 pocoes de vida
            jogador3->inventario.listarItens();                             // Listando o inventario do artifice depois de criar o item
            jogador3->atacar(jogador2);                                     // Artifice ataca o ladrao (se tiver pedra, usa ela, se nao vai no tapa)

            // Removendo item do inventario
            cout << "\n--- REMOVENDO ITEM ---" << endl;
            try{
                jogador1->inventario.removerItem("Pocao de Fogo");    // Remove a pocao de fogo do inventario do Bruxo
                jogador1->inventario.removerItem("Item Inexistente"); // Tenta remover um item que nao existe
            }
            catch (runtime_error &e){
                cout << e.what() << endl; // Da erro porque o item nao existe
            }

            // Desequipando itens
            cout << "\n--- DESEQUIPANDO ---" << endl;
            espada->desequipar(jogador2);        // Ladrao larga a espada de volta pro inventario
            armaduraFerro->desequipar(jogador4); // Guerreiro tira a armadura de volta pro inventario

            // ---------Testando o Singleton (GerenciadorDeJogo, Mercadao e QuadroMissoes)----------
            cout << "\n--- SINGLETON ---" << endl;
            try{
                Mercadao().venderEspolio(espada); // Vendendo a espada no mercadao (adiciona ouro no cofrinho do grupo)
                Mercadao().comprarEspolio(foice); // Comprando a foice (desconta ouro do cofrinho do grupo)
                gerenciadorJogo.exibir();         // Exibindo o estado do gerenciador de jogo
            }
            catch (runtime_error &e){
                cout << e.what() << endl; // Se nao tiver grana suficiente, cai aqui
            }

            // Testando o QuadroMissoes
            cout << "\n--- QUADRO DE MISSOES ---" << endl;
            QuadroMissoes quadro;
            quadro.verificarMissoes(); // Verifica se alguma missao foi concluida

            // Testando inimigos pela Factory
            cout << "\n--- INIMIGOS ---" << endl;
            shared_ptr<Inimigo> dragao = CriacaoInimigos::criarInimigos("dragao");
            shared_ptr<Inimigo> basilisco = CriacaoInimigos::criarInimigos("basilisco");
            shared_ptr<Inimigo> saqueadores = CriacaoInimigos::criarInimigos("saqueadores");

            gerenciadorJogo.adicionarInimigos(dragao); // Adicionando inimigos no gerenciador
            gerenciadorJogo.adicionarInimigos(basilisco);
            gerenciadorJogo.adicionarInimigos(saqueadores);

            // Testando a Masmorra (combate em grupo)
            cout << "\n--- MASMORRA ---" << endl;
            Masmorra().entrar(); // O grupo todo entra na masmorra e escolhe um inimigo pra enfrentar

            // Verificando missoes de novo depois do combate com inimigos
            cout << "\n--- VERIFICANDO MISSOES APOS COMBATE ---" << endl;
            quadro.verificarMissoes(); // Agora pode ter alguma missao concluida (se matou o dragao ou basilisco)
            try{
                quadro.retirarRecompensa("Cacador de Dragoes", jogador4);    // Guerreiro tenta retirar a recompensa de matar o dragao
                quadro.retirarRecompensa("Exterminador de Feras", jogador1); // Bruxo tenta retirar a recompensa de matar o basilisco
            }
            catch (runtime_error &e){
                cout << e.what() << endl; // Se nao tiver recompensa pra tirar, da erro
            }

            // XP ganhado pelo grupo
            cout << "\n--- XP ---" << endl;
            jogador1->ganharXP(50); // Dando xp extra pro Bruxo

            // Save/Load do jogo
            cout << "\n--- SAVE/LOAD ---" << endl;
            SistemasArquivos::salvarJogo(jogador1);                                  // Salva o estado do Bruxo em um json
            shared_ptr<Jogador> jogadorCarregado = SistemasArquivos::carregarJogo(); // Carrega o save de volta
            cout << jogadorCarregado << endl;                                        // Printa o jogador carregado pra ver se bateu com o salvo

            // Status final de todos os jogadores
            cout << "\n--- STATUS FINAL ---" << endl;
            cout << jogador1 << endl;
            cout << jogador2 << endl;
            cout << jogador3 << endl;
            cout << jogador4 << endl;

            // Exibindo todos os logs da partida
            cout << "\n--- LOGS DA PARTIDA ---" << endl;
            Logger::instancia().exibirLogs();

            return 0;
        }
