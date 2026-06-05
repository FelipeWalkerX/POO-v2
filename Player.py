import random as r
from abc import ABC, abstractmethod as abs

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

    '''def exibirStatus(self):
        print(f"Classe: {self.classe}, Nome: {self.__nome}, Raça: {self.raça}, Level: {self.__level}, HP: {self.__hp}/{self.__hpMax}, XP: {self.xp}/{self.xpMax}")
'''
    def __str__(self):
        return f"Classe: {self.classe}, Nome: {self.__nome}, Raça: {self.raça}, Level: {self.__level}, HP: {self.__hp}/{self.__hpMax}, XP: {self.xp}/{self.xpMax}"

    def receberDano(self, dano):
        self.__hp -= dano
        if self.__hp <= 0:
            self.__hp = 0
            self.morto = True
            print(f"{self.__nome} morreu!")
            del self

    def curar(self, cura):
        if self.__hp == self.__hpMax:
            print("Nao é possivel se curar, vida já esta cheia")
            return
        
        elif self.__hp <= 0:
            print(f"{self.__nome} esta morto, ache uma pedra de resurreicao")
            return
        
        self.__hp += cura
        if self.__hp > self.__hpMax:
            self.__hp = self.__hpMax
            print("Vida totalmente restaurada")
            
    def ganharXP(self, experiencia):
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
        self.mana = 100
        self.carisma = 20
        

    def atacar(self, alvo):
        if self.mana >= 10:
            self.mana -= 10
            roll = r.randint(1, 20)
            if roll == 20:
                alvo.receberDano(self.carisma * 2) #Se critar, da o dobro de dano
            elif roll == 1:
                print(f"{self.getNome()} errou o ataque") #Se der falha critica, erra o ataque
                return 0
            else:
                alvo.receberDano(self.carisma)
        else:
            return 10

    def exibirStatus(self):
        print(f"Classe: Bruxo, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")
        
class Ladrao(Jogador):
    def __init__(self, nome, nivel, raça, hp, xp):
        super().__init__(nome, nivel, raça, hp, xp)
        self.agilidade = 10
        self.furtividade = 15
        self.dano = 15

    def atacar(self, alvo):
        roll = r.randint(1, 20)
        if roll == 20:
            alvo.receberDano(self.dano * 4) #Se critar, da o 4x mais dano
            print("SUCESSO CRITICO")
        elif roll == 1:
            print(f"{self.getNome()} errou o ataque")
            return 0
        elif roll > 12:
            alvo.receberDano(self.dano * 2)
        else:
            alvo.receberDano(self.dano)

    def exibirStatus(self):
        print(f"Classe: Ladrao, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")

class Artifice(Jogador):
    def __init__(self, nome, nivel, raça, hp=100, xp = 0):
        super().__init__(nome, nivel, raça, hp, xp)
        self.mana = 100
        self.inteligencia = 20

    def criarItem(self, item):
        if r.randint(0, 100) < 30:
            print("Voce criou uma pedra inutil, parabens =) ")
        else:
            print(f"Voce criou {item} com sucesso")

    def atacar(self, alvo):
        if self.mana >= 10:
            self.mana -= 10
            roll = r.randint(1, 20)
            if roll == 20:
                alvo.receberDano(self.inteligencia * 2) #Se critar, da o dobro de dano
            elif roll == 1:
                print(f"{self.getNome()} errou o ataque")
            else:
                alvo.receberDano(self.inteligencia)
        else:
            return 10
    
    def exibirStatus(self):
        print(f"Classe: Artifice, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")

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

#Itens
class Item:
    def __init__(self, nomeItem, valor, espaco):
        self.nomeItem = nomeItem
        self.valor = valor
        self.espaco = espaco

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

    @abs
    def usarItem(self):
        pass

    def __str__(self):
        return f"{self.nomeItem}, {self.valor}, {self.espaco}. {self.quantidade}"

class PocaoVida(Consumiveis):
    def __init__(self, nomeItem, valor, espaco, quantidade):
        super().__init__(nomeItem, valor, espaco, quantidade)

    def usarItem(self, alvo):
        if self.quantidade > 0:
            alvo.curar(20)  #Chama a funcao curar la de Jogador e cura um jogador 
            self.quantidade -= 1    #Diminui a quantidade dessa pocao
            print(f"Voce usou {self.nomeItem} em {alvo.getNome()} com sucesso!")     #Nao precisa herdar tudo de Jogador, pois o alvo ja faz isso, so precisa chamar o getter por conta do nome ser privado
            return True #Validar que deu certo usar o item
        else: 
            raise ValueError("Voce nao tem mais esta pocao!")
        
class PocaoMana(Consumiveis):
    def __init__(self, nomeItem, valor, espaco, quantidade):
        super().__init__(nomeItem, valor, espaco, quantidade)

    def usarItem(self, alvo):
        if self.quantidade > 0:
            alvo.mana += 20  #Aumenta a mana do jogador
            self.quantidade -= 1    #Diminui a quantidade dessa pocao
            print(f"Voce usou {self.nomeItem}!") 
            return True #Validar que deu certo usar o item
        else: 
            raise ValueError("Voce nao tem mais esta pocao!")
        
class PocaoDanoElemental(Consumiveis):
    def __init__(self, nomeItem, valor, espaco, quantidade, dano):
        super().__init__(nomeItem, valor, espaco, quantidade)
        self.dano = dano

    def usarItem(self, alvo):
        if self.quantidade > 0:
            alvo.receberDano(self.dano)  #Recebe o dano de cada pocao especifica
            self.quantidade -= 1    #Diminui a quantidade dessa pocao
            print(f"Voce usou {self.nomeItem}!") 
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


#Main
if __name__ == "__main__":
    jogador1 = Bruxo("Lucien", 5, MeioElfo(), 48, 0)
    jogador2 = Ladrao("Sevras", 5, MeioElfo(), 35, 0)
    jogador3 = Artifice("Jimothy", 5, Tiefling(), 37, 0)
    jogador4 = Bruxo("Gustav", 4, Humano(), 30, 0)
    
    ''' Ver se ta herdando direito
    print(isinstance(jogador1, Bruxo))
    print(isinstance(jogador1, Jogador))   
    '''
    
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
    
    try:
        jogador3.usarItem()
    except ValueError as e:
        print(e)
    

