package main

import (
	"C"
	"fmt"
	"math"
)

func calculateY(x float64) float64 {
	if x < -3.0 {

		return x + 3.0
	} else if x <= 0.0 {
		valInsideSqrt := 9.0 - x*x
		if valInsideSqrt < 0.0 {

			return math.NaN()
		}
		return math.Sqrt(valInsideSqrt)
	} else if x <= 6.0 {
		return -0.5*x + 3.0
	} else {
		return x - 6.0
	}
}

func main() {
	xStart := -5.0
	xEnd := 9.0
	dx := 1.0

	fmt.Println("+-------+-----------+")
	fmt.Println("|   X   |     Y     |")
	fmt.Println("+-------+-----------+")

	epsilon := dx / 2.0
	for x := xStart; x <= xEnd+epsilon; x += dx {
		y := calculateY(x)
		if math.IsNaN(y) {
			fmt.Printf("| %5.1f | %9s |\n", x, "NaN")
		} else {
			fmt.Printf("| %5.1f | %9.4f |\n", x, y)
		}
	}

	fmt.Println("+-------+-----------+")
}
