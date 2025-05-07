package main

import (
	"fmt"
	"math/big"
	"math/rand"
	"sort"
	"strings"
	"time"
)

// Modular multiplication for big.Int
func modularMultiply(a, b, m *big.Int) *big.Int {
	if m.Sign() == 0 {
		panic("Error: Modulus m cannot be zero")
	}
	var result big.Int
	result.Mul(a, b)
	result.Mod(&result, m)
	return &result
}

// Modular exponentiation for big.Int
func modularPow(base, exp, mod *big.Int) *big.Int {
	var res big.Int
	res.SetInt64(1)
	if mod.Sign() == 0 {
		panic("Error: Modulus m cannot be zero")
	}
	if mod.Cmp(big.NewInt(1)) == 0 {
		return big.NewInt(0)
	}
	base = new(big.Int).Mod(base, mod)
	if base.Sign() == 0 {
		if exp.Sign() == 0 {
			return big.NewInt(1)
		}
		return big.NewInt(0)
	}
	for exp.BitLen() > 0 {
		if exp.Bit(0) == 1 {
			res = modularMultiply(&res, base, mod)
		}
		base = modularMultiply(base, base, mod)
		exp = new(big.Int).Rsh(exp, 1)
	}
	return &res
}

// Prime factorization
func primeFactors(num *big.Int) []*big.Int {
	if num.Cmp(big.NewInt(1)) <= 0 {
		return nil
	}
	factors := make(map[int64]bool)
	n := new(big.Int).Set(num)
	two := big.NewInt(2)
	for n.Mod(n, two).Sign() == 0 {
		factors[2] = true
		n.Div(n, two)
	}
	limit := new(big.Int).Sqrt(n)
	for i := big.NewInt(3); i.Cmp(limit) <= 0; i.Add(i, two) {
		for n.Mod(n, i).Sign() == 0 {
			factors[i.Int64()] = true
			n.Div(n, i)
			limit.Sqrt(n)
		}
		if limit.Cmp(i) < 0 {
			break
		}
	}
	if n.Cmp(big.NewInt(1)) > 0 {
		factors[n.Int64()] = true
	}
	var result []*big.Int
	for k := range factors {
		result = append(result, big.NewInt(k))
	}
	sort.Slice(result, func(i, j int) bool { return result[i].Cmp(result[j]) < 0 })
	return result
}

// Miller-Rabin filter
func millerRabinFilter(n *big.Int, iterations int) bool {
	if n.Cmp(big.NewInt(2)) < 0 {
		return false
	}
	if n.Cmp(big.NewInt(2)) == 0 || n.Cmp(big.NewInt(3)) == 0 {
		return true
	}
	if n.Bit(0) == 0 {
		return false
	}
	d := new(big.Int).Sub(n, big.NewInt(1))
	s := 0
	for d.Bit(0) == 0 {
		d.Rsh(d, 1)
		s++
	}
	rng := rand.New(rand.NewSource(time.Now().UnixNano() + 1))
	for i := 0; i < iterations; i++ {
		a := new(big.Int)
		if n.Cmp(big.NewInt(4)) <= 0 {
			a.SetInt64(2)
		} else {
			a.Rand(rng, new(big.Int).Sub(n, big.NewInt(2)))
			if a.Cmp(big.NewInt(2)) < 0 {
				a.SetInt64(2)
			}
		}
		x := modularPow(a, d, n)
		if x.Cmp(big.NewInt(1)) == 0 || x.Cmp(new(big.Int).Sub(n, big.NewInt(1))) == 0 {
			continue
		}
		foundMinusOne := false
		for r := 1; r < s; r++ {
			x = modularMultiply(x, x, n)
			if x.Cmp(big.NewInt(1)) == 0 {
				return false
			}
			if x.Cmp(new(big.Int).Sub(n, big.NewInt(1))) == 0 {
				foundMinusOne = true
				break
			}
		}
		if !foundMinusOne {
			return false
		}
	}
	return true
}

// Main primality test with factors
func mainTestWithFactors(n *big.Int, kBases int, factors []*big.Int, usedBases *[]*big.Int) bool {
	*usedBases = nil
	if len(factors) == 0 && n.Cmp(big.NewInt(3)) > 0 {
		fmt.Printf("\nWarning: Empty factor list for n=%v, test impossible.\n", n)
		return false
	}
	rng := rand.New(rand.NewSource(time.Now().UnixNano() + 2))
	for i := 0; i < kBases; i++ {
		a := new(big.Int).Rand(rng, new(big.Int).Sub(n, big.NewInt(1)))
		if a.Cmp(big.NewInt(2)) < 0 {
			a.SetInt64(2)
		}
		*usedBases = append(*usedBases, new(big.Int).Set(a))
		if modularPow(a, new(big.Int).Sub(n, big.NewInt(1)), n).Cmp(big.NewInt(1)) != 0 {
			continue
		}
		condition2Holds := true
		for _, q := range factors {
			if new(big.Int).Mod(new(big.Int).Sub(n, big.NewInt(1)), q).Sign() != 0 {
				fmt.Printf("\nLogical error: factor q=%v does not divide n-1=%v\n", q, new(big.Int).Sub(n, big.NewInt(1)))
				condition2Holds = false
				break
			}
			exponent := new(big.Int).Div(new(big.Int).Sub(n, big.NewInt(1)), q)
			if modularPow(a, exponent, n).Cmp(big.NewInt(1)) == 0 {
				condition2Holds = false
				break
			}
		}
		if condition2Holds {
			return true
		}
	}
	return false
}

func main() {
	const numExperimentsTarget = 10
	const mainTestBases = 5
	const filterIterations = 5
	const useFilter = true
	const maxAttempts = numExperimentsTarget * 5000

	experimentNumbers := []int{}
	numbersTested := []*big.Int{}
	results := []byte{}
	iterationsUsed := []int{}
	allUsedBases := [][]*big.Int{}

	rng := rand.New(rand.NewSource(time.Now().UnixNano()))
	minP, maxP := int64(2), int64(500)

	fmt.Printf("Conducting %d experiments:\n", numExperimentsTarget)
	fmt.Println("   (Factorization may take time...)\n")

	successfulExperiments := 0
	attempts := 0

	for successfulExperiments < numExperimentsTarget && attempts < maxAttempts {
		attempts++
		var p int64
		for {
			p = minP + rng.Int63n(maxP-minP+1)
			if p >= 5 && p%2 != 0 {
				break
			}
		}
		pBig := big.NewInt(p)
		fmt.Printf("Attempt %d: P = %d ...", attempts, p)

		if useFilter {
			if !millerRabinFilter(pBig, filterIterations) {
				fmt.Println(" [M-R Filter: Composite]")
				continue
			}
			fmt.Print(" [M-R Filter: Passed] ->")
		}

		fmt.Print(" Factorizing P-1...")
		factors := primeFactors(new(big.Int).Sub(pBig, big.NewInt(1)))
		if len(factors) == 0 && pBig.Cmp(big.NewInt(3)) > 0 {
			fmt.Println(" [Factorization: Error/Empty. Skipping P]")
			continue
		}
		fmt.Print(" [Factorization: Completed] ->")

		fmt.Printf(" Main test (K=%d)...", mainTestBases)
		var currentUsedBases []*big.Int
		mainTestResult := mainTestWithFactors(pBig, mainTestBases, factors, &currentUsedBases)

		successfulExperiments++
		experimentNumbers = append(experimentNumbers, successfulExperiments)
		numbersTested = append(numbersTested, new(big.Int).Set(pBig))
		results = append(results, map[bool]byte{true: '+', false: '-'}[mainTestResult])
		iterationsUsed = append(iterationsUsed, mainTestBases)
		allUsedBases = append(allUsedBases, currentUsedBases)

		fmt.Print(map[bool]string{true: " [Main test: Passed (+)]", false: " [Main test: Failed (-)]"}[mainTestResult])
		fmt.Print(" Bases a: {")
		for j, b := range currentUsedBases {
			fmt.Print(b)
			if j < len(currentUsedBases)-1 {
				fmt.Print(", ")
			}
		}
		fmt.Println("}")
	}

	if successfulExperiments < numExperimentsTarget {
		fmt.Fprintf(&strings.Builder{}, "\n\nWarning: Failed to conduct %d successful experiments in %d attempts.\n", numExperimentsTarget, maxAttempts)
		fmt.Println("Possibly, the P generation range is too small, contains few primes, or factorization is too slow/unsuccessful.")
	}

	fmt.Printf("\n\nFinal results table (%d experiments):\n", successfulExperiments)
	fmt.Println("----------------------------------------------------------------------------------------------------")
	fmt.Printf("| %-3s | %-10s | %-15s | %-5s | %-50s |\n", "N", "P", "Result (+/-)", "K", "Used bases 'a' (from main test)")
	fmt.Println("|-----|------------|-----------------|-------|----------------------------------------------------|")

	for i := range experimentNumbers {
		var basesStr strings.Builder
		basesStr.WriteString("{")
		if len(allUsedBases[i]) > 0 {
			for j, b := range allUsedBases[i] {
				basesStr.WriteString(b.String())
				if j < len(allUsedBases[i])-1 {
					basesStr.WriteString(", ")
				}
			}
		}
		basesStr.WriteString("}")

		resultStr := "(+) Likely prime"
		if results[i] == '-' {
			resultStr = "(-) Composite"
		}

		fmt.Printf("| %-3d | %-10s | %-15s | %-5d | %-50s |\n",
			experimentNumbers[i], numbersTested[i], resultStr, iterationsUsed[i], basesStr.String())
	}
	fmt.Println("----------------------------------------------------------------------------------------------------")
	fmt.Println("* N     - successful experiment number.")
	fmt.Println("* P     - number tested by the main test.")
	fmt.Println("* (+/-) - main test result (using P-1 factors).")
	fmt.Printf("* K     - number of random bases 'a' used in the main test (here = %d).\n", mainTestBases)
	fmt.Println("* Bases 'a' - list of bases tested in the main test for this P.")
}
