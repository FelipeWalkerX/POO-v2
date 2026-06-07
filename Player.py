import random as r
from abc import ABC, abstractmethod as abs

class Inventario:
    def __init__(self, capacidade = 20):    #Limite maximo do inventario
        self.itens = [] #Lista de itens dentro do inventario
        self.capacidade = capacidade
        
    def adicionarItem(self, item):
        totalEspaco = 0         
        for i in self.itens:         #Vai percorrendo o valor do espaco dos itens e salvando em totalEspaco
            totalEspaco += (i.quantidade + 8) // 9      #Vai separando a cada 9 itens estacados no inventario, ele usa mais um espaco para guardar
        if totalEspaco + (item.quantidade + 8) // 9 > self.capacidade:   #Ele ve se a quantidade do proximo item + o espaco que ja ta ocupado vai ser maior que a capacidade maxima
            raise ValueError (f"Inventario Lotado! Capacidade maxima: {totalEspaco}/{self.capacidade}")
        else:
            self.itens.append(item)     #Coloca o item recebido no vetor de itens
            print(f"Item {item.nomeItem} adicionado com sucesso")
            return True
        
    def removerItem(self, nomeItem):
        for item in self.itens:             #Ele faz isso enquanto procura o item na lista de itens
            if item.nomeItem == nomeItem:   #Se o nome do item chamado for o mesmo nome do item que ta no inventario do jogador
                self.itens.remove(item)     #Ele remove o item
                print(f"{nomeItem} removido do inventario")
                return True
        raise ValueError(f"O item {nomeItem} nao foi localizado ou nao existe")
        
        #Mesmo que o item nao exista, ele ainda vai pegar o nome do item digitado, salvar na variavel "nomeItem" e depois comparar pra ver se ele existe, 
        #ai como ja ta salvo na variavel, se nao achar, ele printa o item que ta salvo na variavel e retorna a mensagem de erro

    def listarItens(self):
        print("Inventario: ")
        totalEspaco = 0
        i = 1
        for item in self.itens:     #Vai buscando o item na variavel itens
            pilhas = (item.quantidade + 8) // 9     #Faz uma pilha de itens, salvando quantos itens tem em pilhados, tipo, se tem 10 unidades de um item, ele empilha 9 e sobre 1, ai...
            totalEspaco += pilhas   #Ja ocupa mais 1 espaco no inventario a cada 9 estacados
            print(f"{i}. {item.nomeItem}, quantidade: {item.quantidade}")  #Printando todos os itens no inventario
            i += 1
        print(f" \nEspaco total: {totalEspaco}/{self.capacidade}")   

class Jogador(ABC):
    def __init__(self, nome, level, raça, hp, xp):
        self.__nome = nome
        self.__level = level
        self.__hp = hp
        self.__hpMax = hp
        self.xp = xp
        self.xpMax = 100
        self.morto = False
        self.raça = raça
        #@self.classe = classe
        self.inventario = Inventario()

#Declarando os getters
    def getNome(self):
        return self.__nome
    
    def getLevel(self):
        return self.__level
    
    def getHP(self):
        return self.__hp
    
    def getHPMax(self):
        return self.__hpMax

#Declarando os setters
    def setNome(self, nome):
        self.__nome = nome

    def setLevel(self, level):
        self.__level = level

    def setHP(self, hp):
        self.__hp = hp

    def setHPMax(self, hpMax):
        self.__hpMax = hpMax

    @abs
    def exibirStatus(self):
        pass

    def __str__(self):
        return f"Classe: {self.__class__.__name__}, Nome: {self.__nome}, Raça: {self.raça}, Level: {self.__level}, HP: {self.__hp}/{self.__hpMax}, XP: {self.xp}/{self.xpMax}"

    def receberDano(self, dano):
        self.__hp -= dano
        if self.__hp <= 0:
            self.__hp = 0
            self.morto = True
            del self

    def curar(self, cura):
        if self.__hp == self.__hpMax:
            print("Nao é possivel se curar, vida já esta cheia")
            return False
        
        elif self.__hp <= 0:
            print(f"{self.__nome} esta morto, ache um orbe da resurreicao")
            return False
        
        self.__hp += cura
        if self.__hp > self.__hpMax:
            self.__hp = self.__hpMax
            print("Vida totalmente restaurada")
            
    def ganharXP(self, experiencia):
        if self.morto == True:
            print(f"{self.__nome} morreu e nao pode ganhar xp!")
            return False
        self.xp += experiencia
        while self.xp >= self.xpMax:
            self.__level += 1
            self.xp -= self.xpMax
        print(f"{self.__nome} subiu para o nivel {self.__level}")

    @abs
    def atacar(self):
        raise NotImplementedError("Subclasse deve implementar atacar")
        
    def __del__(self):
        if self.morto == True:
            print(f"Game Over para {self.__nome}")

#Classes
class Bruxo(Jogador):
    def __init__(self, nome, nivel, raça, hp = 80, xp = 0):
        super().__init__(nome, nivel, raça, hp, xp)
        self.classe = "Bruxo"
        self.mana = 100
        self.carisma = 20
        

    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False
        
        if self.mana >= 10:
            self.mana -= 10
            roll = r.randint(1, 20)
            if roll == 20:
                alvo.receberDano(self.carisma * 2) #Se critar, da o dobro de dano
                print(f"{self.getNome()} critou e deu {(self.carisma * 2)} de dano em {alvo.getNome()}")
            elif roll == 1:
                print(f"{self.getNome()} errou o ataque em {alvo.getNome()}") #Se der falha critica, erra o ataque
                return 0
            else:
                alvo.receberDano(self.carisma)
                print(f"{self.getNome()} deu {(self.carisma)} de dano em {alvo.getNome()}")

            if alvo.morto:
                print(f"{alvo.getNome()} morreu!\n")
            else:
                print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")
        else:
            return 10

    def exibirStatus(self):
        print(f"Classe: {self.classe}, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")
        
class Ladrao(Jogador):
    def __init__(self, nome, nivel, raça, hp = 100, xp = 0):
        super().__init__(nome, nivel, raça, hp, xp)
        self.classe = "Ladrao"
        self.agilidade = 10
        self.furtividade = 15
        self.dano = 15

    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False
        
        roll = r.randint(1, 20)
        if roll == 20:
            alvo.receberDano(self.dano * 4) #Se critar, da o 4x mais dano
            print("SUCESSO CRITICO")
            print(f"{self.getNome()} deu {(self.dano * 4)} de dano em {alvo.getNome()}")

        elif roll == 1:
            print(f"{self.getNome()} errou o ataque em {alvo.getNome()}")
            return 0
        elif roll > 12:
            alvo.receberDano(self.dano * 2)
            print(f"{self.getNome()} critou e deu {(self.dano * 2)} de dano em {alvo.getNome()}")
            
        else:
            alvo.receberDano(self.dano)
            print(f"{self.getNome()} deu {(self.dano)} de dano em {alvo.getNome()}")

        if alvo.morto:
            print(f"{alvo.getNome()} morreu!\n")
        else:
            print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")
            

    def exibirStatus(self):
        print(f"Classe: {self.classe}, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")

class Artifice(Jogador):
    def __init__(self, nome, nivel, raça, hp=100, xp = 0):
        super().__init__(nome, nivel, raça, hp, xp)
        self.classe = "Artifice"
        self.mana = 100
        self.inteligencia = 20

    def criarItem(self, item):
        if r.randint(0, 100) < 30:
            print("Voce criou uma pedra inutil, parabens =) ")
        else:
            print(f"Voce criou {item} com sucesso")

    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False

        if self.mana >= 10:
            self.mana -= 10
            roll = r.randint(1, 20)
            if roll == 20:
                alvo.receberDano(self.inteligencia * 2) #Se critar, da o dobro de dano
                print(f"{self.getNome()} critou e deu {(self.inteligencia * 2)} de dano em {alvo.getNome()}")

            elif roll == 1:
                print(f"{self.getNome()} errou o ataque em {alvo.getNome()}")
            else:
                alvo.receberDano(self.inteligencia)
                print(f"{self.getNome()} deu {(self.inteligencia)} de dano em {alvo.getNome()}")

            if alvo.morto:
                print(f"{alvo.getNome()} morreu!\n")
            else:
                print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")

        else:
            return 10
    
    def exibirStatus(self):
        print(f"Classe: {self.classe}, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")

class Raça:
    def __init__(self, bonusCarisma = 0, bonusInteligencia = 0, bonusAgilidade = 0):
        self.bonusCarisma = bonusCarisma
        self.bonusInteligencia = bonusInteligencia
        self.bonusAgilidade = bonusAgilidade
        #self.bonusConstituição = bonusConstituição

    #Mudar pra ele mostrar o nome da raca e nao o endereco de memoria de onde ta
    def __str__(self):
        return self.__class__.__name__

#Raças
class MeioElfo(Raça):
    def __init__(self):
        super().__init__(bonusCarisma = 2, bonusInteligencia = 2, bonusAgilidade = 2)

class Humano(Raça):
    def __init__(self):
        super().__init__(bonusCarisma=2, bonusInteligencia=2, bonusAgilidade=2)

class Tiefling(Raça):
    def __init__(self):
        super().__init__(bonusCarisma = 4, bonusInteligencia = 2)

class Item:
    def __init__(self, nomeItem, valor, espaco):
        self.nomeItem = nomeItem
        self.valor = valor
        self.espaco = espaco

#Itens
class Arma(Item):
    def __init__(self, nomeItem, valor, espaco, dano):
        super().__init__(nomeItem, valor, espaco)
        self.dano = dano

    #Passar pra string
    def __str__(self):
        return f"{self.nomeItem}, {self.valor}, {self.espaco}. {self.dano}"

class Escudo(Item):
    def __init__(self, nomeItem, valor, espaco, durabilidade, defesa):
        super().__init__(nomeItem, valor, espaco)
        self.durabilidade = durabilidade
        self.defesa = defesa

    def __str__(self):
        return f"{self.nomeItem}, {self.valor}, {self.espaco}. {self.durabilidade}, {self.defesa}"

class Consumiveis(Item, ABC):   #Herdando de Item de de ABC (para fazer o usar item ser um metodo abstrado)
    def __init__(self, nomeItem, valor, espaco, quantidade):
        super().__init__(nomeItem, valor, espaco)
        self.quantidade = quantidade

    @abs    #Polimorfismo e classe abstrata (pois uso o usarItem pra diversas coisas)
    def usarItem(self):
        pass

    def __str__(self):  #Usando polimorfismo
        return f"{self.nomeItem}, {self.valor}, {self.espaco}. {self.quantidade}"

#Pocoes
class PocaoVida(Consumiveis):
    def __init__(self, quantidade = 0):
        super().__init__(nomeItem = "Pocao de Vida", valor = 60, espaco = 1, quantidade = quantidade)

    def usarItem(self, usuario, alvo):
        if self.quantidade > 0:
            if alvo.morto or usuario.morto:
                if usuario != alvo:
                    if alvo.morto:
                        print(f"{alvo.getNome()} esta morto e nao pode receber cura")
                        return False
                    elif usuario.morto:
                        print(f"{usuario.getNome()} esta morto e nao pode curar {alvo.getNome()}")
                        return False
                else: 
                    print(f"{usuario.getNome()} esta morto e nao pode se curar")
                    return False

            alvo.curar(20)  #Chama a funcao curar la de Jogador e cura um jogador 
            self.quantidade -= 1    #Diminui a quantidade dessa pocao
            if usuario.getNome() == alvo.getNome():
                print(f"{usuario.getNome()} se curou! || HP: {usuario.getHP()}/{usuario.getHPMax()}")
                return True

            print(f"{usuario.getNome()} usou {self.nomeItem} em {alvo.getNome()} com sucesso! || HP: {alvo.getHP()}/{alvo.getHPMax()}")     #Nao precisa herdar tudo de Jogador, pois o alvo ja faz isso, so precisa chamar o getter por conta do nome ser privado
            return True #Validar que deu certo usar o item
        else: 
            raise ValueError("Voce nao tem mais esta pocao!")
        
class PocaoMana(Consumiveis):
    def __init__(self, quantidade = 0):
        super().__init__(nomeItem = "Pocao de Mana", valor = 60, espaco = 1, quantidade = quantidade)

    def usarItem(self, usuario):
        if self.quantidade > 0:
            if usuario.morto:
                print(f"{usuario.getNome()} esta morto e nao pode recuperar mana")
                return False
            usuario.mana += 20  #Aumenta a mana do jogador
            if usuario.mana >100:
                usuario.mana = 100
            self.quantidade -= 1    #Diminui a quantidade dessa pocao
            print(f"{usuario.getNome()} usou {self.nomeItem}! || Mana: {usuario.mana}") 
            return True #Validar que deu certo usar o item
        else: 
            raise ValueError("Voce nao tem mais esta pocao!")
        
class PocaoDanoElemental(Consumiveis):
    def __init__(self, nomeItem, valor, espaco, quantidade, dano):
        super().__init__(nomeItem, valor, espaco, quantidade)
        self.dano = dano

    def usarItem(self, usuario, alvo):
        if self.quantidade > 0:
            if usuario.morto:
                print(f"{usuario.getNome()} esta morto e nao pode usar item")
                return False
            self.quantidade -= 1    #Diminui a quantidade dessa pocao
            alvo.receberDano(self.dano)  #Recebe o dano de cada pocao especifica
            print(f"{usuario.getNome()} usou {self.nomeItem} em {alvo.getNome()}! || {alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()} HP") 
            #print(f"{self.getNome()} usou {self.nomeItem} em {alvo.getNome()}!") 

            return True #Validar que deu certo usar o item
        else: 
            raise ValueError("Voce nao tem mais esta pocao!")

#Separando as pocoes de dano por elementos        
class DanoFogo(PocaoDanoElemental):
    def __init__(self, quantidade = 0):
        super().__init__(nomeItem = "Pocao de Fogo", valor = 50, espaco = 1, quantidade = quantidade, dano = 50) #Setando cada valor defaut da pocao

class DanoGelo(PocaoDanoElemental):
    def __init__(self, quantidade = 0):
        super().__init__(nomeItem = "Pocao de Gelo", valor = 40, espaco = 1, quantidade = quantidade, dano = 30)

class DanoRelampago(PocaoDanoElemental):
    def __init__(self, quantidade = 0):
        super().__init__(nomeItem = "Pocao de Relampago", valor = 60, espaco = 2, quantidade = quantidade, dano = 70)

class DanoRadiante(PocaoDanoElemental):
    def __init__(self, quantidade = 0):
        super().__init__(nomeItem = "Pocao Radiante", valor = 999, espaco = 1, quantidade = quantidade, dano = 50)

class DanoVenenoso(PocaoDanoElemental):
    def __init__(self, quantidade = 0):
        super().__init__(nomeItem = "Pocao Venenosa", valor = 100, espaco = 1, quantidade = quantidade, dano = 40)

class OrbeRessureicao(Consumiveis):
    def __init__(self, quantidade = 0):
        super().__init__(nomeItem = "Orbe da Ressureicao", valor = 1000, espaco = 3, quantidade = quantidade)

    def usarItem(self, usuario, alvo):
        if alvo != usuario:
            if usuario.morto:
                print(f"{usuario.getNome()} tambem esta morto e nao pode ressucitar {alvo.getNome()}")
                return False
            if self.quantidade > 0:
                alvo.morto = False
                alvo.setHP(1)       #Setando o hp de volta pra 1 pra nao cair na malha fina do curar
                self.quantidade -= 1
                print(f"{alvo.getNome()} foi ressucitado por {usuario.getNome()} || {alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}")
                return True
            else: 
                raise ValueError("Voce nao tem mais orbes!")
        else:
            print(f"{alvo.getNome()} ainda esta vivo")
            return False

if __name__ == "__main__":
    # Criando jogadores
    jogador1 = Bruxo("Lucien", 5, MeioElfo(), 48, 0)
    jogador2 = Ladrao("Sevras", 5, MeioElfo(), 35, 0)
    jogador3 = Artifice("Jimothy", 5, Tiefling(), 37, 0)

    # Criando itens
    pocaoVida = PocaoVida(3)
    pocaoMana = PocaoMana(2)
    fogoItem = DanoFogo(2)
    geloItem = DanoGelo(1)
    orbeItem = OrbeRessureicao(1)

    # Adicionando itens no inventario
    jogador1.inventario.adicionarItem(pocaoVida)
    jogador1.inventario.adicionarItem(pocaoMana)
    jogador1.inventario.adicionarItem(fogoItem)
    jogador3.inventario.adicionarItem(orbeItem)
    jogador1.inventario.listarItens()

    print("\n--- STATUS ---")
    print(jogador1)
    print(jogador2)
    print(jogador3)

    print("\n--- COMBATE ---")
    jogador1.atacar(jogador2)
    jogador2.atacar(jogador1)
    jogador3.atacar(jogador1)

    print("\n--- USANDO ITENS ---")
    try:
        pocaoVida.usarItem(jogador1, jogador1)
        pocaoMana.usarItem(jogador1)
        fogoItem.usarItem(jogador1, jogador2)   #(usuario, alvo)
        orbeItem.usarItem(jogador3, jogador1)

    except ValueError as e:
        print(e)

    print("\n--- REMOVENDO ITEM ---")
    try:
        jogador1.inventario.removerItem("Pocao de Fogo")
        jogador1.inventario.removerItem("Item Inexistente")
    except ValueError as e:
        print(e)

    print("\n--- STATUS FINAL ---")
    print(jogador1)
    print(jogador2)

    print("\n--- XP ---")
    jogador1.ganharXP(150)
    print(jogador1)

'''
#Main
if __name__ == "__main__":
    jogador1 = Bruxo("Lucien", 5, MeioElfo(), 48, 0)
    jogador2 = Ladrao("Sevras", 5, MeioElfo(), 35, 0)
    jogador3 = Artifice("Jimothy", 5, Tiefling(), 37, 0)
    jogador4 = Bruxo("Gustav", 4, Humano(), 30, 0)
    
    #Ver se ta herdando direito
    print(isinstance(jogador1, Bruxo))
    print(isinstance(jogador1, Jogador))   
    
    
    jogador1.curar(15)
    print(jogador1)
    jogador1.atacar(jogador2)

    print("\n")
    
    jogador2.receberDano(19)
    jogador2.ganharXP(110)
    print(jogador2)
    print("\n")

    jogador3.receberDano(39)
    print(jogador3)
    jogador3.curar(10)

    print("\n")

    jogador4.receberDano(29)
    print(jogador4)
    jogador4.curar(10)
    print(jogador4)
    
    pocaoVida = PocaoVida(11)
    jogador3.inventario.adicionarItem(pocaoVida)
    jogador3.inventario.listarItens()


    try:
        pocaoVida.usarItem(jogador2)
    except ValueError as e:
        print(e)
'''
