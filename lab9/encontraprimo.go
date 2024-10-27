package main

import (
	"fmt"
	"math"
)

// Função dada
func ehPrimo(n int) bool {
	if n <= 1 {
		return false
	}
	if n == 2 {
		return true
	}
	if n%2 == 0 {
		return false
	}
	for i := 3; float64(i) < math.Sqrt(float64(n))+1; i += 2 {
		if n%i == 0 {
			return false
		}
	}
	return true
}

// Conta primos em um intervalo
func tarefa(job int, tag int, results chan int, done chan bool) {
	c := 0
	for i := job; i < (job + tag + 1); i++ {
		if ehPrimo(i) {
			c++
		}
	}
	results <- c //Envia o resultado para o buffer
	done <- true // Sinaliza que a goroutine terminou
}

func main() {
	N := 9000000 //Tamanho de sequência
	M := 4       //Quantidade de threads

	if N%M > 0 {
		println("N deve ser múltiplo de M")
		return
	}

	results := make(chan int, M) // Canal com os resultados
	done := make(chan bool, M)   // Canal para rastrear goroutines concluídas

	tag := N / M // Tamanho do intervalo

	for i := 0; i < M; i++ {
		ini := i * tag
		go tarefa(ini, tag, results, done)
	}

	// Goroutine para fechar o canal results após todas as tarefas terminarem
	go func() {
		for i := 0; i < M; i++ {
			<-done // Espera todos os sinais de conclusão
		}
		close(results)
	}()

	primos := 0
	for count := range results { //Soma os resultados
		primos += count
	}

	fmt.Printf("Total de números primos de 1 a %d: %d\n", N, primos)
}
