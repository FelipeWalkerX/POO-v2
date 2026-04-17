#include<iostream>
#include<string>
#include<vector>

using namespace std;

class Animal{
    private:
        string nome;

    public:
        void setName(string nome){
            this-> nome = nome;
        }
        string getName(){
            return this -> nome;
        }
        virtual string emitirSom(){ //virtual: É usado para sobreescrever o "return"
            return "...";
        }
};

class Cachorro: public Animal{
    public:
        virtual string emitirSom(){
            return "Au-Au";
        }

};

class Gato: public Animal{
    public:
        Gato(){

        }
        Gato(string nome){
            setName(nome);
        }

        virtual string emitirSom(){
            return "Meow";
        }

};

int main(){

    Gato* j = new Gato();
    j->setName("Jessica");

    /*Gato* p = new Gato();
    p->setName("Patricia");

    Gato* f = new Gato();
    f->setName("Fernanda");*/
    
    Gato* p = new Gato("Patricia");
    Gato* f = new Gato("Fernanda");

    Cachorro* a = new Cachorro();
    a->setName("Arthur");

    cout << "Eu tenho gatas chamadas " + j->getName() + ", " + p->getName() + ", " + f->getName() + " e um cachorro chamado " + a->getName() << endl;

    vector <Animal*> v;
    v.push_back(new Gato());
    v.push_back(new Cachorro());

    for(size_t i = 0; i < v.size(); i++){
        cout << v[i]->emitirSom() << endl;
    }

    return 0;
}
