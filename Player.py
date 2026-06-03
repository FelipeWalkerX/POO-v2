import random as r
from abc import ABC, abstractmethod as abs

class Jogador(ABC):
    def __init__(self, nome, level, hp, xp):
        self.__nome = nome
        self.__level = level
        self.__hp = hp
        self.__hpMax = hp
        self.xp = xp
        self.xpMax = 100
        self.morto = False

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

    def exibirStatus(self):
        print(f"Nome: {self.__nome}, Level: {self.__level}, HP: {self.__hp}/{self.__hpMax}, XP: {self.xp}/{self.xpMax}")

    def receberDano(self, dano):
        self.__hp -= dano
        if self.__hp <= 0:
            self.__hp = 0
            self.morto = "True"
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
        if self.morto == "True":
            print(f"Game Over para {self.__nome}")

class Bruxo(Jogador):
    def __init__(self, nome, nivel, hp = 80, xp = 0):
        super().__init__(nome, nivel, hp, xp)
        self.mana = 100
        self.constituiçao = 20
        

    def atacar(self):
        if self.mana >= 10:
            self.mana -= 10
            return self.constituiçao * 2
        else:
            return 10

    def exibirStatus(self):
        print(f"Classe: Bruxo, Nome: {self.getNome()}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")
        
class Ladrao(Jogador):
    def __init__(self, nome, nivel, hp=100, xp = 0):
        super().__init__(nome, nivel, hp, xp)
        self.agilidade = 10
        self.furtividade = 15
        self.dano = 15

    def atacar(self):
        if r.randint(1, 20) > 12:
            return self.dano * 3
        else:
            return 10

    def exibirStatus(self):
        print(f"Classe: Ladrao, Nome: {self.getNome()}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")


class Artifice(Jogador):
    def __init__(self, nome, nivel, hp=100, xp = 0):
        super().__init__(nome, nivel, hp, xp)
        self.mana = 100
        self.inteligencia = 20

    def criarItem(self, item):
        if r.randint(0, 100) < 30:
            print("Voce criou uma pedra inutil, parabens =) ")
        else:
            print(f"Voce criou {item} com sucesso")

    def atacar(self):
        if self.mana >= 10:
            self.mana -= 10
            return self.inteligencia * 2
        else:
            return 10
    
    def exibirStatus(self):
        print(f"Classe: Artifice, Nome: {self.getNome()}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")


if __name__ == "__main__":
    jogador1 = Bruxo("Lucien", 5, 48, 0)
    jogador2 = Ladrao("Sevras", 5, 35, 0)
    jogador3 = Artifice("Jimothy", 5, 37, 0)
    jogador4 = Bruxo("Gustav", 4, 30, 0)
    print(isinstance(jogador1, Bruxo))
    print(isinstance(jogador1, Jogador))
    
    jogador1.curar(10)
    jogador1.exibirStatus()
    print("\n")
    
    jogador2.receberDano(19)
    jogador2.ganharXP(110)
    jogador2.exibirStatus()
    print("\n")

    jogador3.receberDano(39)
    jogador3.exibirStatus()
    jogador3.curar(10)

    print("\n")

    jogador4.receberDano(29)
    jogador4.exibirStatus()
    jogador4.curar(10)
    jogador4.exibirStatus()
    
    

