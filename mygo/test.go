package main

import (
	"bufio"
	"fmt"
	"net/http"
	"net/url"
	"os"
	"time"
)

func main() {
	// url := "http://example.com/textfile.txt"

	file, err := os.Open("/usr/local/gitcode/ss.txt")
	if err != nil {
		fmt.Println(err)
		return
	}
	defer file.Close()
	// Create a scanner to read the file line by line
	scanner := bufio.NewScanner(file)
	// Iterate over each line and print it
	for scanner.Scan() {
		line := scanner.Text()
		fmt.Println(line)
		u, err := url.Parse(line)
		if err != nil {
			fmt.Print("wrong linke", err)
			continue
		}
		domain := u.Hostname()
		fmt.Print(domain)
		start := time.Now()
		resp, err := http.Get("http://" + domain)
		if err != nil {
			fmt.Println("Error making HTTP request:", err)
			continue
		}
		defer resp.Body.Close()
		elapsed := time.Since(start)
		fmt.Printf("Time taken to get response from %s: %v\n", domain, elapsed)
	}

	// Check for errors during scanning
	if err := scanner.Err(); err != nil {
		fmt.Println(err)
		return
	}

}
