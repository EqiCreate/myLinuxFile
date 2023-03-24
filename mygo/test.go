// package main

// import (
// 	"bufio"
// 	"fmt"
// 	"net"
// 	"net/url"
// 	"os"
// 	"sort"
// 	"time"
// )

// type ipTime struct {
// 	Ip   *net.IPAddr
// 	time time.Duration
// }

// func main() {

// 	file, err := os.Open("/usr/local/gitcode/ss.txt")
// 	if err != nil {
// 		fmt.Println(err)
// 		return
// 	}
// 	defer file.Close()

// 	var ipTimes []ipTime
// 	// Create a scanner to read the file line by line
// 	scanner := bufio.NewScanner(file)
// 	// Iterate over each line and print it
// 	for scanner.Scan() {
// 		line := scanner.Text()
// 		// fmt.Println(line)
// 		u, err := url.Parse(line)
// 		if err != nil {
// 			fmt.Print("wrong linke", err)
// 			continue
// 		}
// 		domain := u.Hostname()
// 		fmt.Print(domain)
// 		start := time.Now()
// 		// resp, err := http.Get("http://" + domain)
// 		ip := net.ParseIP(domain)
// 		if ip != nil {
// 			fmt.Printf("direct ip = %s \n", ip.String())
// 		} else {
// 			ip, err := net.ResolveIPAddr("ip", domain)
// 			if err != nil {
// 				fmt.Printf("error resolving domain: %v\n", err)
// 				continue
// 			}
// 			// conn, err := net.DialIP("ip4:icmp", nil, ip)
// 			conn, err := net.Dial("tcp", u.Host)
// 			if err != nil {
// 				fmt.Println("DialIP Error:", err)
// 				continue
// 			}
// 			elapsed := time.Since(start)
// 			fmt.Printf("Time taken to get response from %s: %v\n", domain, elapsed)
// 			ipTimes = append(ipTimes, ipTime{Ip: ip, time: elapsed})
// 			defer conn.Close()

// 		}

// 	}

// 	// Check for errors during scanning
// 	if err := scanner.Err(); err != nil {
// 		fmt.Println(err)
// 		return
// 	}
// 	sort.Slice(ipTimes, func(i, j int) bool {
// 		return ipTimes[i].time < ipTimes[j].time
// 	})

// 	for _, ipTime := range ipTimes {
// 		fmt.Printf("%s took %v dial \n", ipTime.Ip, ipTime.time)
// 	}

// }
