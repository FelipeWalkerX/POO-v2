from __future__ import annotations      #Nao tirar da primeira linha, pode dar bosta
import random as r
from abc import ABC, abstractmethod as abs
import json
from pathlib import Path

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

    def buscarItens(self, nomeItem):
        for item in self.itens:
            if item.nomeItem == nomeItem:
                return item
        raise ValueError(f"Item {nomeItem} nao encontrado!")

class Jogador(ABC):
    def __init__(self, nome, level, raça, hp, xp, armadura = 0):
        self.__nome = nome
        self.__level = level
        self.__hp = hp
        self.__hpMax = hp
        self.xp = xp
        self.xpMax = 100
        self.morto = False
        self.raça = raça
        #self.classe = classe
        self.inventario = Inventario()
        self.armaEquipada = None
        self.armaduraBase = armadura
        self.armaduraEquipada = None
        self.defesaTotal = 0

#Declarando os getters
    def getNome(self):
        return self.__nome
    
    def getLevel(self):
        return self.__level
    
    def getHP(self):
        return self.__hp
    
    def getHPMax(self):
        return self.__hpMax
    
    def getArmadura(self):
        bonus = 0
        if self.armaduraEquipada is not None:
            bonus = self.armaduraEquipada.defesa
        return self.armaduraBase + bonus

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

    def receberDano(self, dano, matador=None):    #Passa o jogador que matar o inimigo (o que der o ultimo hit) para ganhar o xp
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
        gerenciadorJogo = GerenciadorDeJogo.instancia()
        gerenciadorJogo.adicionarXP(experiencia)
        while self.xp >= self.xpMax:
            self.__level += 1
            self.xp -= self.xpMax
            gerenciadorJogo.subirNivel()
        print(f"{self.__nome} subiu para o nivel {self.__level}")

    @abs
    def atacar(self):
        raise NotImplementedError("Subclasse deve implementar atacar")
    
    def serializaDicionario(self):  #Cria um ponto de save
        return  {'nome': self.getNome(),
                'level': self.getLevel(),
                'hp': self.getHP(),
                'xp': self.xp,
                #'inventario': self.inventario,
                'raça': self.raça.__class__.__name__,   #Precisa passar cada nome de raca
                'classe': self.__class__.__name__,  #E de classe
                'versao': '1.0'}

    @classmethod
    def desserializaDicionario(cls, dados):   #Volta no save (reconstroi o objeto do mesmo jeito que foi construido no ponto de save)
        j = cls(dados['nome'], dados['level'], dados['raça'], dados['hp'], dados['xp']) #Polimorfismo: usa o cls para nao precisar fazer um pra cada classse, entao ele reutiliza a mesma copisa para qualquer classe
        j.inventario = dados.get('inventario')
        return j

    def __str__(self):
        return f"Classe: {self.__class__.__name__}, Nome: {self.__nome}, Raça: {self.raça}, Level: {self.__level}, HP: {self.__hp}/{self.__hpMax}, XP: {self.xp}/{self.xpMax}"

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
        #Nao colocado para equipar arma pois ele tem foco arcano que anda com ele

    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False
        
        if self.mana >= 10:
            self.mana -= 10
            roll = r.randint(1, 20)

            if alvo.getArmadura() > roll:
                print(f"A armadura de {alvo.getNome()} absorveu todo dano")

            if roll == 20:
                alvo.receberDano(self.carisma * 2, self) #Se critar, da o dobro de dano // Passando o self de novo para o jogo saber quem matou o inimigo e entregar a recompensa
                Logger().log(f"{self.getNome()} critou e deu {(self.carisma * 2)} de dano em {alvo.getNome()}")   #Chamando a funcao "Logger" pra ja registrar no log e ja printa tambem
            elif roll == 1:
                Logger().log(f"{self.getNome()} errou o ataque em {alvo.getNome()}") #Se der falha critica, erra o ataque
                return 0
            else:
                alvo.receberDano(self.carisma, self)
                Logger().log(f"{self.getNome()} deu {(self.carisma)} de dano em {alvo.getNome()}")

            if alvo.morto:
                print(f"{alvo.getNome()} morreu!\n")
            else:
                print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")
        else:
            raise ValueError(f"{self.getNome()} nao tem mais mana! Mana restante: {self.mana}")
            #return 10          #Dano base tirado no tapa

    def exibirStatus(self):
        print(f"Classe: {self.classe}, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")
        
class Ladrao(Jogador):
    def __init__(self, nome, nivel, raça, hp = 100, xp = 0):
        super().__init__(nome, nivel, raça, hp, xp)
        self.classe = "Ladrao"
        self.agilidade = 10
        self.furtividade = 15
        self.dano = 15

        #Movido pra classe Jogador:
        #self.defesaTotal = 0
        #self.armaEquipada = None
        #self.armaduraEquipada = None

    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False
        
        if self.armaEquipada is not None:
            roll = r.randint(1, 20)
            if alvo.getArmadura() > roll:
                print(f"A armadura de {alvo.getNome()} absorveu todo dano")

            if roll == 20:
                alvo.receberDano(self.dano * 4, self) #Se critar, da o 4x mais dano
                #print("SUCESSO CRITICO")
                Logger().log(f"SUCESSO CRITICO!!! {self.getNome()} deu {(self.dano * 4)} de dano em {alvo.getNome()}")

            elif roll == 1:
                Logger().log(f"{self.getNome()} errou o ataque em {alvo.getNome()}")
                return 0
            elif roll > 12:
                alvo.receberDano(self.dano * 2, self)
                Logger().log(f"{self.getNome()} critou e deu {(self.dano * 2)} de dano em {alvo.getNome()}")
                
            else:
                alvo.receberDano(self.dano, self)
                Logger().log(f"{self.getNome()} deu {(self.dano)} de dano em {alvo.getNome()}")

            if alvo.morto:
                print(f"{alvo.getNome()} morreu!\n")
            else:
                print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")

        else:
            print(f"{self.getNome()} estava sem arma equipada para atacar")
            Logger().log(f"{self.getNome()} deu um tapa em {alvo.getNome()}")
            return 10       #Dano rancado no tapa
            

    def exibirStatus(self):
        print(f"Classe: {self.classe}, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")

class Artifice(Jogador):
    def __init__(self, nome, nivel, raça, hp=100, xp = 0):
        super().__init__(nome, nivel, raça, hp, xp)
        self.classe = "Artifice"
        self.mana = 100
        self.inteligencia = 20
        #self.armaEquipada = None

    def criarItem(self, tipo, quantidade = 1):
        if self.morto is True:
            print(f"{self.getNome()} esta morto e nao pode criar itens")
            return None
        
        if r.randint(0, 100) < 30:
            pedra = ItemGenerico("Pedra Inutil", valor = 0, espaco = 1)     #Ele faz uma pedra
            self.inventario.adicionarItem(pedra)            #E guarda a porcaria da pedra no inventario ksksksk
            print(f"{self.getNome()} criou uma pedra inutil, parabens =) ")
            return pedra
        
        
        item = CriacaoItens.criarPocao(tipo, quantidade) or CriacaoItens.criarArma(tipo)        #Tenta criar o que o jogador solicitou (um item ou uma arma), ele nao pode criar armaduras

        if item is None:        #Se o item nao existir no jogo ou no ItemFactory
            raise ValueError(f"{self.getNome()} nao sabe como criar {tipo}")
        
        self.inventario.adicionarItem(item)     #So depois de validar se o item existe, ele guarda o item criado no inventario
        print(f"Voce criou {item.nomeItem} com sucesso")
        return item

        

    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False
        
        if self.armaEquipada is not None:
            if self.mana >= 10:
                self.mana -= 10
                roll = r.randint(1, 20)

                if alvo.getArmadura() > roll:
                    print(f"A armadura de {alvo.getNome()} absorveu todo dano")

                if roll == 20:
                    alvo.receberDano(self.inteligencia * 2, self) #Se critar, da o dobro de dano
                    Logger().log(f"{self.getNome()} critou e deu {(self.inteligencia * 2)} de dano em {alvo.getNome()}")

                elif roll == 1:
                    Logger().log(f"{self.getNome()} errou o ataque em {alvo.getNome()}")
                else:
                    alvo.receberDano(self.inteligencia, self)
                    Logger().log(f"{self.getNome()} deu {(self.inteligencia)} de dano em {alvo.getNome()}")

                if alvo.morto:
                    print(f"{alvo.getNome()} morreu!\n")
                else:
                    print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")

            else:
                print(f"{self.getNome()} nao tem mais mana! Mana restante: {self.mana}")
                
        else:
            try:
                pedra = self.inventario.buscarItens("Pedra Inutil")
                pedra.quantidade -= 1
                if pedra.quantidade < 1:
                    self.inventario.removerItem("Pedra Inutil")     #Se acabou as pedras do inventario, remove e libera espaco
                alvo.receberDano(10, self)
                Logger().log(f"{self.getNome()} tacou uma pedra em {alvo.getNome()}")
            except:
                Logger().log(f"{self.getNome()} deu um tapa em {alvo.getNome()}")
                return 5   #Dano rancado no tapa
    
    def exibirStatus(self):
        print(f"Classe: {self.classe}, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")

class Guerreiro(Jogador):
    def __init__(self, nome, nivel, raça, hp = 150, xp = 0):
        super().__init__(nome, nivel, raça, hp, xp, 5)
        self.classe = "Guerreiro"

        self.forca = 20
        #self.defesaTotal = 0
        #self.armaEquipada = None
        #self.armaduraEquipada = None

    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False
        
        if self.armaEquipada  is not None:      #Se a arma nao tiver equipada, se fode e da o dano default
            roll = r.randint(1, 20)
            if alvo.getArmadura() > roll:
                print(f"A armadura de {alvo.getNome()} absorveu todo dano")

            if roll == 20:
                alvo.receberDano(self.forca * 2, self)
                print("SUCESSO CRITICO")
                Logger().log(f"{self.getNome()} deu {(self.forca * 2)} de dano em {alvo.getNome()}")

            elif roll == 1:
                Logger().log(f"{self.getNome()} errou o ataque em {alvo.getNome()}")
                return 0
              
            else:
                alvo.receberDano(self.forca, self)
                Logger().log(f"{self.getNome()} deu {(self.forca)} de dano em {alvo.getNome()}")

            if alvo.morto:
                print(f"{alvo.getNome()} morreu!\n")
            else:
                print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")

        else:
            print(f"{self.getNome()} estava sem arma equipada para atacar")
            Logger().log(f"{self.getNome()} deu um murro em {alvo.getNome()}")
            return 20       #Dano rancado no murro
            

    def exibirStatus(self):
        print(f"Classe: {self.classe}, Nome: {self.getNome()}, Raça: {self.raça}, Level: {self.getLevel()}, HP: {self.getHP()}/{self.getHPMax()}, XP: {self.xp}/{self.xpMax}")

#Inimigos
class Inimigo(ABC): #Classe so pra inimigo
    def __init__(self, nome, hp, dano, xpRecompensa, ouroRecompensa):
        self.__nome = nome
        self.hp = hp
        self.hpMax = hp
        self.dano = dano
        self.xpRecompensa = xpRecompensa
        self.ouroRecompensa = ouroRecompensa
        self.morto = False

    def getNome(self):
        return self.__nome
    
    def setNome(self, nome):
        self.__nome = nome

    @abs
    def atacar(self, alvo): #Aonde os inimigos irao herdar o atacar
        pass

    def droparRecompensas(self, jogador): #Funcao que ativa caso o inimigo morra, ai ele libera a recompensas para os jogadores
        if self.morto:
            jogador.ganharXP(self.xpRecompensa)
            GerenciadorDeJogo().adicionarOuro(self.ouroRecompensa) #Grana ganha no combate com qualquer inimigo vai para o cofrinho do time
            print(f"{jogador.getNome()} ganhou {self.xpRecompensa} de XP e {self.ouroRecompensa} de grana!")

    def receberDano(self, dano, jogador):
        self.hp -= dano
        if self.hp <= 0:
            self.hp = 0
            self.morto = True
            print(f"{self.getNome()} foi derrotado!")
            self.droparRecompensas(jogador)

class Dragao(Inimigo):
    def __init__(self):
        super().__init__("Dragao", hp = 3000, dano = 30, xpRecompensa = 100, ouroRecompensa = 200)

    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False
        roll = r.randint(1, 20)
        if roll == 1:
            Logger().log(f"{self.getNome()} falhou miseravelmente no seu ataque")
            return 0
        alvo.receberDano(self.dano, self)
        Logger().log(f"{self.getNome()} soltou o bafo do selvagem no {alvo.getNome()} e deu {self.dano} de dano")
        if alvo.morto:
            print(f"{alvo.getNome()} virou carvao!\n")
        else:
            print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")

class Basilisco(Inimigo):
    def __init__(self):
        super().__init__("Basilisco", hp = 2000, dano = 25, xpRecompensa = 80, ouroRecompensa = 120)
  
    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False
        roll = r.randint(1, 20)
        if roll == 1:
            Logger().log(f"{self.getNome()} falhou miseravelmente no seu ataque")
            return 0
        alvo.receberDano(self.dano, self)
        Logger().log(f"{self.getNome()} mordeu {alvo.getNome()} e deu {self.dano} de dano")
        if alvo.morto:
            print(f"{alvo.getNome()} foi engolido pelo {self.getNome()}!\n")
        else:
            print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")

class Saqueadores(Inimigo):
    def __init__(self):
        super().__init__("Saqueadores", hp = 500, dano = 15, xpRecompensa = 30, ouroRecompensa = 100)

    def atacar(self, alvo):
        print(f"{self.getNome()} vai atacar {alvo.getNome()}:")
        if alvo.morto:
            print(f"{alvo.getNome()} ja esta morto!")
            return False
        roll = r.randint(1, 20)
        if roll == 1:
            Logger().log(f"{self.getNome()} falhou miseravelmente no seu ataque")
            return 0
        alvo.receberDano(self.dano, self)
        Logger().log(f"{self.getNome()} esfaqueou o bucho do {alvo.getNome()} e deu {self.dano} de dano")
        if alvo.morto:
            print(f"{alvo.getNome()} virou estatistica!\n")
        else:
            print(f"{alvo.getNome()}: {alvo.getHP()}/{alvo.getHPMax()}\n")

class Raça:
    def __init__(self, bonusCarisma = 0, bonusInteligencia = 0, bonusAgilidade = 0):
        self.bonusCarisma = bonusCarisma
        self.bonusInteligencia = bonusInteligencia
        self.bonusAgilidade = bonusAgilidade
        #self.bonusConstituição = bonusConstituição

    #Mudar pra ele mostrar o nome da raça e nao o endereco de memoria de onde ta
    def __str__(self):
        return self.__class__.__name__

#Raças
class MeioElfo(Raça):
    def __init__(self):
        super().__init__(bonusCarisma = 4, bonusInteligencia = 2, bonusAgilidade = 2)

class Humano(Raça):
    def __init__(self):
        super().__init__(bonusCarisma=2, bonusInteligencia=2, bonusAgilidade=2)

class Tiefling(Raça):
    def __init__(self):
        super().__init__(bonusCarisma = 4, bonusInteligencia = 2)

class Item(ABC):
    def __init__(self, nomeItem, valor, espaco):
        self.nomeItem = nomeItem
        self.valor = valor
        self.espaco = espaco

    @abs
    def equipar(self, jogador):
        pass
    
    @abs
    def desequipar(self, jogador):
        pass

#Itens
class Arma(Item):
    def __init__(self, nomeItem, valor, espaco, dano):
        super().__init__(nomeItem, valor, espaco)
        self.quantidade = 1
        self.dano = dano

    def equipar(self, jogador):
        if jogador.armaEquipada is None:   #Se a arma nao tiver equipada
            jogador.armaEquipada = self     #Pego a arma  
            jogador.dano = self.dano
            jogador.inventario.itens.remove(self)   #Remove a arma do inventario
            print(f"{jogador.getNome()} equipou a {self.nomeItem}")
        else:
            print(f"{jogador.getNome()}, ja esta com a {self.nomeItem} equipada!")

    def desequipar(self, jogador):
        if jogador.armaEquipada is not None:       #Se a arma tiver equipada
            jogador.armaEquipada = None         #Guarda a arma
            jogador.inventario.adicionarItem(self)        #Coloca o item de volta no inventario, voltando a ocupar espaco
            jogador.dano = 0
            print(f"{jogador.getNome()} largou a {self.nomeItem}")
        else:
            print(f"{jogador.getNome()}, ja guardou a {self.nomeItem}!")

    #Passar pra string
    def __str__(self):
        return f"{self.nomeItem}, {self.valor}, {self.espaco}. {self.dano}"

class Armadura(Item):
    def __init__(self, nomeItem, valor, espaco, defesa):
        super().__init__(nomeItem, valor, espaco)
        self.quantidade = 1
        self.defesa = defesa

    def equipar(self, jogador):
        if jogador.armaduraEquipada is None:    #Se o JOGADOR estiver sem arma, ele roda esse bloco e nao o self (que nesse caso seria a propria arma)
            jogador.armaduraEquipada = self     #Coloco a armadura
            jogador.defesaTotal += self.defesa  
            jogador.inventario.itens.remove(self)          #Zero o valor que ele ocupa no inventario (ou seja, tiro a armadura do inventario para coloca-la)
            print(f"{jogador.getNome()} vestiu sua {self.nomeItem}")
        else:
            print(f"{jogador.getNome()}, ja esta com a {self.nomeItem} equipada!")

    def desequipar(self, jogador):
        if jogador.armaduraEquipada is not None:        #Mesma coisa que para colocar a arma
            jogador.armaduraEquipada = None     #Tira a armadura
            jogador.defesaTotal -= self.defesa      #Perde a defesa extra
            jogador.inventario.adicionarItem(self)          #Coloca o item de volta no inventario, voltando a ocupar espaco
            print(f"{jogador.getNome()} tirou a {self.nomeItem}")
        else:
            print(f"{jogador.getNome()}, ja esta sem a {self.nomeItem}!")


    def __str__(self):
        return f"{self.nomeItem}, {self.valor}, {self.espaco}, {self.defesa}"
        
class Consumiveis(Item, ABC):   #Herdando de Item e de ABC (para fazer o usar item ser um metodo abstrado)
    def __init__(self, nomeItem, valor, espaco, quantidade):
        super().__init__(nomeItem, valor, espaco)
        self.quantidade = quantidade

    def equipar(self, jogador):
        pass
    
    def desequipar(self, jogador):
        pass

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
            if usuario.morto is False:
                print(f"{alvo.getNome()} ainda esta vivo")
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
            print("Nao é permitido usar o orbe em si mesmo")
            return False

class ItemGenerico(Item):
    def __init__(self, nomeItem, valor = 0, espaco = 1, quantidade = 1):
        super().__init__(nomeItem, valor, espaco)
        self.quantidade = quantidade

    def equipar(self, jogador):
        pass
    
    def desequipar(self, jogador):
        pass

    def __str__(self):
        return f"{self.nomeItem} inutil"

#Singleton (vai ler e escrever tudo no mesmo lugar, seja na masmorra, mercado ou quadro de missao)
class GerenciadorDeJogo:
    _instancia: GerenciadorDeJogo | None = None     #Ou a instancia recebe gerenciador de jogo ou None

    def __new__(cls) -> GerenciadorDeJogo:       #Cria o objeto antes do __init__
        if cls._instancia is None:
            cls._instancia = super().__new__(cls)       #Chama o new da classe pai (cria o objeto)
            cls._instancia._inicializado = False        #Fala que a instancia ainda nao foi incializado 
            cls._instancia.jogadores = []       #Cria uma lista de jogadores
            cls._instancia.inimigos = []        #Cria uma lista de inimigos
        
        return cls._instancia       #Retorna a mesma instancia pra tudo
    
    def __init__(self):
        if self._inicializado:      #Ve se ja foi iniciado antes
            return      #Se sim, sai sem fazer nada e impede do init setar de novo o ouro, xp e nivel pra 0
        #Se nao, seta nivel em 1 e o resto em 0
        self.nivel = 1
        self.ouro = 0
        self.xp = 0
        self._inicializado = True   #E inicia a instancia

    @classmethod
    def instancia(cls) -> GerenciadorDeJogo:       #Serve pra instanciar um, objeto ou classe
        return cls()    #Cria e retorna a instancia

    def adicionarOuro(self, quantidade: int):
        self.ouro += quantidade

    def comprarItem(self, item):
        if self.ouro < item.valor:
            raise ValueError(f"Ta liso paizao? Tu so tem {self.ouro} e a {item.nomeItem} custa {item.valor}")
        self.ouro -= item.valor

    def adicionarXP(self, xp: int):
        self.xp += xp

    def subirNivel(self):
        self.nivel += 1

    def exibir(self):
        print(f"[Gerenciador]: nivel = {self.nivel} "
                             f"ouro  = {self.ouro} "
                             f"xp = {self.xp}")
        
    def adicionarJogador(self, jogador):
        if len(self.jogadores) > 5:
            raise ValueError(f"Quantidade de jogadores limite atingida!!! Total {len(self.jogadores)}/5")
        self.jogadores.append(jogador)
        print(f"{jogador.getNome()} adicionado. Total de jogadores: {len(self.jogadores)}")

    def adicionarInimigos(self, inimigo):
      self.inimigos.append(inimigo)
      print(f"{inimigo.getNome()} adicionado. Total de inimigos: {len(self.inimigos)}")
        
class Masmorra:
    def derrotarChefe(self):
        gerenciadorJogo = GerenciadorDeJogo()   #Chama o gerenciador de jogo (singleton)
        gerenciadorJogo.adicionarXP(100)
        gerenciadorJogo.adicionarOuro(50)
        gerenciadorJogo.subirNivel()
        print(f"[Masmorra]: Chefe derrotado!")
        gerenciadorJogo.exibir()

class Mercadao:
    def venderEspolio(self, item): 
        gerenciadorJogo = GerenciadorDeJogo()           #Chama o singleton (gerenciador de jogo)
        gerenciadorJogo.adicionarOuro(item.valor)       #E vende o item
        print(f"[Mercadao]: {item} foi barganhado!")
        gerenciadorJogo.exibir()

    def comprarEspolio(self, item):
        gerenciadorJogo = GerenciadorDeJogo()
        gerenciadorJogo.comprarItem(item)               #Tenta comprar o item se tiver dinheiro
        print(f"[Mercadao]: {item} foi barganhado!")
        gerenciadorJogo.exibir()

class QuadroMissoes:
    def completarMissao (self, nome: str):
        gerenciadorJogo = GerenciadorDeJogo()
        gerenciadorJogo.adicionarXP(200)
        gerenciadorJogo.adicionarOuro(50)
        print(f"[Missao]: {nome} concluida!")
        gerenciadorJogo.exibir()

class Logger:
    _instancia = None     #Recebendo None como default

    def __new__(cls) -> Logger:       #Cria o objeto antes do __init__
        if cls._instancia is None:
            cls._instancia = super().__new__(cls)       #Chama o new da classe pai (cria o objeto)
            cls._instancia._inicializado = False        #Fala que a instancia ainda nao foi incializado 
        return cls._instancia       #Retorna a mesma instancia pra tudo
    
    def __init__(self):
        if self._inicializado:      #Ve se ja foi iniciado antes
            return      #Se sim, sai sem fazer nada e impede do init setar de novo o ouro, xp e nivel pra 0
        #Se nao, seta nivel em 1 e o resto em 0
        self.logs = []
        self._inicializado = True

    def registrarMensagem(self, msg):
      self.logs.append(msg)

    def exibirLogs(self):
      for log in self.logs:
        print(f"{log}")

    def log(self, msg):
      print(msg)
      self.logs.append(msg)

#Factory
class CriacaoItens:
    
    @staticmethod
    def criarArma(tipo):
        armas = {"espada": Arma("Dualiso", 200, 2, 25),
                 "machado": Arma("Machadinha Infernal", 300, 2, 40),
                 "adaga": Arma("Lamina Oculta", 150, 1, 20), 
                 "rapiera": Arma("Maellum", 500, 2, 35),
                 "foco arcano": Arma("Trebuchim", 300, 1, 20),
                 "foice": Arma("Moisson", 400, 3, 25)}
        
        return armas.get(tipo, None)

    @staticmethod
    def criarArmaduras(tipo):
        armaduras = {"malha": Armadura("Cota de Malha", 100, 2, 3),
                     "couro": Armadura("Armadura de Couro", 150, 2, 5),
                     "ferro": Armadura("Armadura de Ferro", 400, 3, 10)}
        
        return armaduras.get(tipo, None)

    @staticmethod
    def criarPocao(tipo, quantidade = 1):
        pocoes = {"vida": PocaoVida(quantidade),     #Ja tenho a variavel das pocoes, posso so cria-las agora
                  "mana": PocaoMana(quantidade),
                  "fogo": DanoFogo(quantidade),
                  "gelo": DanoGelo(quantidade),
                  "relampago": DanoRelampago(quantidade),
                  "radiante": DanoRadiante(quantidade),
                  "venenoso": DanoVenenoso(quantidade)}
        
        return pocoes.get(tipo, None)

class CriacaoJogadores:
    @staticmethod
    def criarJogadores(classe, nome, nivel, raça):
        jogadores = {"bruxo": Bruxo(nome, nivel, raça),
                     "ladrao": Ladrao(nome, nivel, raça),
                     "artifice": Artifice(nome, nivel, raça),
                     "guerreiro": Guerreiro(nome, nivel, raça)}
        return jogadores.get(classe, None)

class CriacaoInimigos:
  @staticmethod
  def criarInimigos(nome):
    inimigos = {"dragao": Dragao(), #Nao precisa passar nenhum parametro pois todos ja estao presetados no __init__
                "basilisco": Basilisco(),
                "saqueadores": Saqueadores()}
    return inimigos.get(nome, None)

class SistemasArquivos:
    @staticmethod
    def salvarJogo(jogador, arquivo="save.json"):   #Salvando o jogo em um json
        dados = jogador.serializaDicionario()
        with open(arquivo, 'w', encoding='utf-8') as f:     #Abre o arquivo em modo de escrita ("write") e salva em f
            json.dump(dados, f, indent=2, ensure_ascii=False)   #Salva os dados em f e identa com 2 espacos (ensure_ascii=False é pra permitir salvar caracteres especiais)
        print(f"Jogo salvo em {arquivo}")
    
    @staticmethod
    def carregarJogo(arquivo = "save.json"):
        if not Path(arquivo).exists():      #Se o arquivo "arquivo" nao existir
            print(f"Arquivo {arquivo} nao encontrado!")
            return None
        
        with open(arquivo, 'r', encoding='utf-8') as f:     #Carrega o arquivo em modo leitura ("read") de f
            dados = json.load(f)       #Carrega o json do arquivo f

        classe = dados.get('classe')
        classes = {'Bruxo': Bruxo,
                   'Ladrao': Ladrao,
                   'Artifice': Artifice,
                   'Guerreiro': Guerreiro}

        jogador = classes[classe].desserializaDicionario(dados)     #O jogador recebe os dados do ultimo save, precisa pegar pela classe pois a classe Jogador é abstrata
        print(f"Jogo carregado de {arquivo}")
        return jogador
    
#Main
if __name__ == "__main__":
        # Criando jogadores pela Factory
    jogador1 = CriacaoJogadores.criarJogadores("bruxo", "Lucien", 5, MeioElfo())
    jogador2 = CriacaoJogadores.criarJogadores("ladrao", "Sevras", 5, MeioElfo())
    jogador3 = CriacaoJogadores.criarJogadores("artifice", "Jimothy", 5, Tiefling())
    jogador4 = CriacaoJogadores.criarJogadores("guerreiro", "Tsarin", 5, Humano())

    # Criando itens pela Factory
    pocaoVida = CriacaoItens.criarPocao("vida", 3)
    pocaoMana = CriacaoItens.criarPocao("mana", 2)
    fogoItem = CriacaoItens.criarPocao("fogo", 2)
    geloItem = CriacaoItens.criarPocao("gelo", 1)
    orbeItem = OrbeRessureicao(1)
    espada = CriacaoItens.criarArma("espada")
    foice = CriacaoItens.criarArma("foice")
    armaduraFerro = CriacaoItens.criarArmaduras("ferro")

    # Adicionando itens no inventario
    jogador1.inventario.adicionarItem(pocaoVida)
    jogador1.inventario.adicionarItem(pocaoMana)
    jogador1.inventario.adicionarItem(fogoItem)
    jogador3.inventario.adicionarItem(orbeItem)
    jogador2.inventario.adicionarItem(espada)
    jogador4.inventario.adicionarItem(armaduraFerro)
    jogador1.inventario.listarItens()

    # Equipar arma e armadura
    print("\n--- EQUIPANDO ---")
    espada.equipar(jogador2)
    armaduraFerro.equipar(jogador4)

    print("\n--- STATUS ---")
    print(jogador1)
    print(jogador2)
    print(jogador3)
    print(jogador4)

    print("\n--- COMBATE ---")
    jogador1.atacar(jogador2)
    jogador2.atacar(jogador1)
    jogador3.atacar(jogador1)
    jogador4.atacar(jogador1)

    print("\n--- USANDO ITENS ---")
    try:
        pocaoVida.usarItem(jogador1, jogador1)
        pocaoMana.usarItem(jogador1)
        fogoItem.usarItem(jogador1, jogador2)
        orbeItem.usarItem(jogador3, jogador1)
    except ValueError as e:
        print(e)

    print("\n--- ARTIFICE CRIANDO ITEM E ATACANDO COM PEDRA ---")
    jogador3.criarItem("vida", 2)
    jogador3.inventario.listarItens()
    jogador3.atacar(jogador2)

    print("\n--- REMOVENDO ITEM ---")
    try:
        jogador1.inventario.removerItem("Pocao de Fogo")
        jogador1.inventario.removerItem("Item Inexistente")
    except ValueError as e:
        print(e)

    print("\n--- SINGLETON ---")
    try:
        Masmorra().derrotarChefe()
        Mercadao().venderEspolio(espada)
        Mercadao().comprarEspolio(foice)
        QuadroMissoes().completarMissao("Primeira Masmorra")
    except ValueError as e:
        print(e)

    print("\n--- STATUS FINAL ---")
    print(jogador1)
    print(jogador2)
    print(jogador3)
    print(jogador4)

    print("\n--- XP ---")
    jogador1.ganharXP(150)
    print(jogador1)

    print("\n--- INIMIGOS ---")
    dragao = CriacaoInimigos.criarInimigos("dragao")
    basilisco = CriacaoInimigos.criarInimigos("basilisco")
    gerenciadorJogo = GerenciadorDeJogo.instancia()
    gerenciadorJogo.adicionarInimigos(dragao)
    gerenciadorJogo.adicionarInimigos(basilisco)

    dragao.atacar(jogador1)
    jogador4.atacar(dragao)  # testando inimigo recebendo dano e dropando recompensa

    # Save/Load
    print("\n--- SAVE/LOAD ---")
    SistemasArquivos.salvarJogo(jogador1)
    jogadorCarregado = SistemasArquivos.carregarJogo()
    print(jogadorCarregado)

    # Desequipar
    print("\n--- DESEQUIPANDO ---")
    espada.desequipar(jogador2)
    armaduraFerro.desequipar(jogador4)

    #Exibindo os logs da partida
    print("\n--- LOGS ---")
    Logger().exibirLogs()

