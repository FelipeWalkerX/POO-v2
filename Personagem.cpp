#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;

int rolarDado(int lados) {
    return (rand() % lados) + 1;
}

class Jogador {
private:
string nome;
    int nivel;
    int hp;
    int hp_maximo;
public:
    // Construtor
    Jogador(string nome, int nivel = 1, int hp = 100) {
        this->nome = nome;
        this->hp_maximo = hp;   //hp_maximo precisa vir antes do setHp
        setNivel(nivel);        //Colocado um set aqui para corrigir os valores que forem colocados na main (como o level negativo do j3)
        setHp(hp);              //Validando se o HP é menor que 0 ou maior que o hp maximo
    }

    //Destrutor virtual: evita memory leak ao deletar subclasses por ponteiro base
    virtual ~Jogador() {}

    //Getters
    string getNome(){
        return nome;
    }
    int getNivel(){
        return nivel;
    }
    int getHp(){
        return hp;
    }
    int getHpMaximo(){
        return hp_maximo;
    }
    //Setters
    void setNivel(int nivel){
        if (nivel < 0) 
            this->nivel = 1;
        else
            this->nivel = nivel;        
    }
    void setHp(int hp){
        if(hp < 0)
            this->hp = 0;
        else if(hp > hp_maximo)
            this->hp = hp_maximo;
        else    
            this->hp = hp;
    }
    //Metodos
    virtual void exibir_status() {
        cout << "Nome: " << nome << " | Nível: " << nivel << " | HP: " << hp << "/" << hp_maximo << endl;
    }
    bool esta_vivo() {
        return hp > 0;
    }
    void receber_dano(int dano) {
        hp -= dano;
        if (hp < 0) hp = 0;
        cout << nome << " ficou com " << hp << "/" << hp_maximo << " HP" << endl;  //Mostra HP apenas ao tomar dano
    }
    void curar(int quantidade) {
        hp += quantidade;
        if (hp > hp_maximo) hp = hp_maximo;
    }

    //Metodo abstrato: cada subclasse implementa o seu
    virtual int atacar(Jogador* alvo, int armadura_alvo) = 0;
};

//Guerreiro
class Guerreiro : public Jogador {
private:
    int forca;
public:
    Guerreiro(string nome, int nivel = 1, int hp = 120, int forca = 15)
        : Jogador(nome, nivel, hp) {
        this->forca = forca;
    }

    void exibir_status() override {
        Jogador::exibir_status();
        cout << "Classe: Guerreiro | Força: " << forca << endl;
    }

    int atacar(Jogador* alvo, int armadura_alvo) override {
        int rolagem = rolarDado(20);
        cout << getNome() << " rolou " << rolagem << " (armadura inimigo: " << armadura_alvo << ")" << endl;

        if (rolagem <= armadura_alvo) {
            cout << "Ataque não acertou!" << endl;
            return 0;
        }

        int dano = (int)(forca * 1.5);

        if (rolagem == 20) {
            dano *= 2;
            cout << "CRÍTICO! ";
        }

        cout << getNome() << " causou " << dano << " de dano!" << endl;
        alvo->receber_dano(dano);
        return dano;
    }
};

//Mago
class Mago : public Jogador {
private:
    int inteligencia;
    int mana;
    int mana_maxima;
public:
    Mago(string nome, int nivel = 1, int hp = 80, int inteligencia = 20, int mana = 100)
        : Jogador(nome, nivel, hp) {
        this->inteligencia = inteligencia;
        this->mana = mana;
        this->mana_maxima = mana;
    }

    void exibir_status() override {
        Jogador::exibir_status();
        cout << "Classe: Mago | Inteligência: " << inteligencia << " | Mana: " << mana << "/" << mana_maxima << endl;
    }

    int atacar(Jogador* alvo, int armadura_alvo) override {
        int rolagem = rolarDado(20);
        cout << getNome() << " rolou " << rolagem << " (armadura inimigo: " << armadura_alvo << ")" << endl;

        if (rolagem <= armadura_alvo) {
            cout << "Ataque não acertou!" << endl;
            return 0;
        }

        int dano = 0;

        if (mana >= 10) {
            dano = (int)(inteligencia * 2.0);
            mana -= 10;

            if (rolagem == 20) {
                dano *= 2;
                cout << "CRÍTICO! ";
            }

            cout << getNome() << " lançou magia e causou " << dano << " de dano! (Mana: " << mana << "/" << mana_maxima << ")" << endl;

        } else {
            //Sem mana - jogador escolhe
            cout << "Sem mana! O que deseja fazer?" << endl;
            cout << "1 - Ataque físico (rola 1d6)" << endl;
            cout << "2 - Concentrar (recupera 1/4 da mana máxima)" << endl;

            int escolha;
            cin >> escolha;

            if (escolha == 1) {
                dano = rolarDado(6);
                if (rolagem == 20) {
                    dano *= 2;
                    cout << "CRÍTICO! ";
                }
                cout << getNome() << " atacou fisicamente e causou " << dano << " de dano!" << endl;
            } else {
                int recuperado = mana_maxima / 4;
                mana += recuperado;
                if (mana > mana_maxima) mana = mana_maxima;
                cout << getNome() << " se concentrou e recuperou " << recuperado << " de mana! (Mana: " << mana << "/" << mana_maxima << ")" << endl;
            }
        }

        if (dano > 0) alvo->receber_dano(dano);
        return dano;
    }
};

//Ladrao
class Ladrao : public Jogador {
private:
    int agilidade;
    int chance_critico;
public:
    Ladrao(string nome, int nivel = 1, int hp = 90, int agilidade = 18, int chance_critico = 13)
        : Jogador(nome, nivel, hp) {
        this->agilidade = agilidade;
        this->chance_critico = chance_critico;
    }

    void exibir_status() override {
        Jogador::exibir_status();
        cout << "Classe: Ladrão | Agilidade: " << agilidade << " | Crítico (>=" << chance_critico << "): x3 | Acerto 20: x5" << endl;
    }

    int atacar(Jogador* alvo, int armadura_alvo) override {
        int rolagem = rolarDado(20);
        cout << getNome() << " rolou " << rolagem << " (armadura inimigo: " << armadura_alvo << ")" << endl;

        if (rolagem <= armadura_alvo) {
            cout << "Ataque não acertou!" << endl;
            return 0;
        }

        int dano = rolarDado(6);

        if (rolagem == 20) {
            dano *= 5;
            cout << "ACERTO PERFEITO! ";
        } else if ((rolagem - armadura_alvo) >= 3) {
            dano *= 3;
            cout << "Acerto preciso! ";
        }

        cout << getNome() << " causou " << dano << " de dano!" << endl;
        alvo->receber_dano(dano);
        return dano;
    }
};

int main() {
    srand(time(0));

    Guerreiro j1("Aragorn", 5, 150, 18);
    Mago j2("Gandalf", 8, 90, 22, 100);
    Ladrao j3("Legolas", 4, 100, 20, 13);

    cout << "=== STATUS INICIAL ===" << endl;
    j1.exibir_status();
    cout << endl;
    j2.exibir_status();
    cout << endl;
    j3.exibir_status();
    cout << endl;

    //Vetor de ponteiros - polimorfismo (todos sao Jogador*)
    vector<Jogador*> jogadores = { &j1, &j2, &j3 };

    int armadura_alvo = 10;

    //Percorre a lista e chama atacar() sem saber o tipo real de cada objeto
    cout << "=== COMBATE ===" << endl;
    for (int i = 0; i < jogadores.size(); i++) {
        if (!jogadores[i]->esta_vivo()) continue;

        //Ataca o proximo da lista (ultimo ataca o primeiro)
        int proximo = (i + 1) % jogadores.size();
        cout << "\n" << jogadores[i]->getNome() << " ataca " << jogadores[proximo]->getNome() << endl;
        jogadores[i]->atacar(jogadores[proximo], armadura_alvo);
    }

    return 0;
}
