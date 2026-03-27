class termostato:
  def __init__(self, temperatura=20, arLigado=False):
    self.__temperatura = temperatura
    self.__arLigado = arLigado

  def getTemperatura(self):
    return self.__temperatura

  def getArLigado(self):
    return self.__arLigado

  def ligarAr(self):
    self.__arLigado = True

  def desligarAr(self):
    self.__arLigado = False

  def __validarTemperatura(self, temperatura):
    if temperatura < 0:
      return 0

  def aumentarTemperatura(self, quantidade):
    self.__temperatura += quantidade
    if self.__temperatura > 40:
      print("Temperatura máxima atingida")
      self.__temperatura = 40

  def diminuirTemperatura(self, quantidade):
    self.__temperatura -= quantidade
    if self.__temperatura < 0:
      print("!!!Temperatura mínima atingida!!!")
      self.__temperatura = 0


  def exibirStatus(self):
    print(f"Temperatura: {self.__temperatura}°C")
    if self.__arLigado:
      print("Ar ligado")
    else:
      print("Ar desligado")

if __name__ == "__main__":
  termostato = termostato()
  termostato.exibirStatus()
  
  termostato.aumentarTemperatura(10)
  termostato.ligarAr()
  termostato.exibirStatus()

  termostato.diminuirTemperatura(35)
  termostato.desligarAr()
  termostato.exibirStatus()
