// introduz canais na aplicação Go
package main

import (
	"fmt"
)

func tarefa(str chan string) {
	var msg2 = ""
	//escreve no canal
	msg2 = <-str
	fmt.Println(msg2)
	str <- "Oi Main, bom dia, tudo bem?"
	msg2 = <-str
	fmt.Println(msg2)
	str <- "Certo, entendido."
	msg2 = <-str
	fmt.Println(msg2)
	str <- "finalizando"
}

func main() {
	var msg = "" //string vazia

	//cria um canal de comunicacao nao-bufferizado
	str := make(chan string)

	//cria um goroutine que executará a função 'tarefa'
	go tarefa(str)

	//lê do canal
	str <- "Olá, Goroutine, bom dia!"
	msg = <-str
	fmt.Println(msg)
	str <- "Tudo bem!, Vou terminar tá?"
	msg = <-str
	fmt.Println(msg)
	str <- "finalizando"
	msg = <-str
	fmt.Println(msg)
}
