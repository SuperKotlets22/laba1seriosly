package main

import (
	"bufio"
	"fmt"
	"os"
)

// Вспомогательная функция для реализации deque с использованием среза
type deque []int

func (d *deque) PushBack(x int) {
	*d = append(*d, x)
}

func (d *deque) PopFront() int {
	if len(*d) == 0 {
		panic("pop from empty deque") // Или другая обработка ошибки
	}
	val := (*d)[0]
	*d = (*d)[1:]
	return val
}

func (d *deque) PopBack() int {
	if len(*d) == 0 {
		panic("pop from empty deque") // Или другая обработка ошибки
	}
	n := len(*d)
	val := (*d)[n-1]
	*d = (*d)[:n-1]
	return val
}

func (d *deque) Front() int {
	if len(*d) == 0 {
		panic("front from empty deque") // Или другая обработка ошибки
	}
	return (*d)[0]
}

func (d *deque) Back() int {
	if len(*d) == 0 {
		panic("back from empty deque") // Или другая обработка ошибки
	}
	return (*d)[len(*d)-1]
}

func (d *deque) IsEmpty() bool {
	return len(*d) == 0
}

func main() {
	// Используем буферизованный ввод для ускорения
	reader := bufio.NewReader(os.Stdin)
	// Используем буферизованный вывод для ускорения (хотя для одного числа не критично)
	// writer := bufio.NewWriter(os.Stdout)
	// defer writer.Flush() // Не забыть сбросить буфер в конце

	var n, m int
	fmt.Fscan(reader, &n, &m)

	a := make([]int64, n)
	for i := 0; i < n; i++ {
		fmt.Fscan(reader, &a[i])
	}

	// Вычисляем префиксные суммы
	prefixSum := make([]int64, n+1)
	prefixSum[0] = 0
	for i := 0; i < n; i++ {
		prefixSum[i+1] = prefixSum[i] + a[i]
	}

	// dp[i] = максимальная разница, которую может получить текущий игрок,
	// начиная с индекса i (т.е. рассматривая подмассив a[i:])
	dp := make([]int64, n+1)
	// value[j] = prefixSum[j] - dp[j]
	// Это значение используется для оптимизации поиска максимума
	value := make([]int64, n+1)
	// Дек (двусторонняя очередь) хранит индексы j для оптимизации
	// с помощью скользящего окна максимума по value[j]
	dq := make(deque, 0, n+1) // Инициализируем дек с начальной емкостью

	// Базовый случай: если не осталось элементов (индекс n), разница равна 0
	dp[n] = 0
	value[n] = prefixSum[n] - dp[n]
	dq.PushBack(n) // Добавляем индекс n в дек

	// Итерируемся в обратном порядке от n-1 до 0
	for i := n - 1; i >= 0; i-- {
		// Удаляем индексы, которые вышли из окна [i+1, i+m]
		// Окно представляет возможные конечные позиции j для хода из i (j = i + len, 1 <= len <= m)
		// Индекс j соответствует состоянию после хода, поэтому j >= i+1
		// Максимальный индекс j это i+m
		for !dq.IsEmpty() && dq.Front() > i+m {
			dq.PopFront()
		}

		// dp[i] = max_{i+1 <= j <= min(n, i+m)} (prefixSum[j] - prefixSum[i] - dp[j])
		// Перегруппируем: dp[i] = max_{i+1 <= j <= min(n, i+m)} (prefixSum[j] - dp[j]) - prefixSum[i]
		// Оптимизация: max_{...} (prefixSum[j] - dp[j]) это max_{...} (value[j])
		// Дек хранит индексы j так, что value[dq.Front()] является максимумом в текущем окне
		dp[i] = value[dq.Front()] - prefixSum[i]

		// Вычисляем value[i], которое понадобится для следующих шагов (когда i станет частью окна)
		value[i] = prefixSum[i] - dp[i]

		// Поддерживаем свойство дека: значения value[j] для индексов j в деке
		// должны быть расположены в неубывающем порядке (монотонно убывающая очередь по значениям).
		// Удаляем сзади все индексы k, для которых value[k] <= value[i],
		// так как они никогда не будут максимумом, пока i находится в окне.
		for !dq.IsEmpty() && value[dq.Back()] <= value[i] {
			dq.PopBack()
		}
		// Добавляем текущий индекс i в конец дека
		dq.PushBack(i)
	}

	// dp[0] - это максимальная разница, которую может получить Павел (первый игрок),
	// начиная с самого начала массива.
	// Если dp[0] > 0, Павел может гарантировать себе положительную разницу (выиграть).
	if dp[0] > 0 {
		// fmt.Fprintln(writer, 1) // Используем writer для вывода
		fmt.Println(1) // Вывод 1, если Павел выигрывает
	} else {
		// fmt.Fprintln(writer, 0) // Используем writer для вывода
		fmt.Println(0) // Вывод 0, если Павел не может гарантировать выигрыш
	}
}
