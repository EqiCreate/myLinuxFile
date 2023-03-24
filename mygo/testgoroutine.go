package main

import (
	"bufio"
	"fmt"
	"net"
	"net/url"
	"os"
	"sort"
	"time"
)

type hostResult struct {
	host  string
	delay time.Duration
	err   error
}

type ipTime struct {
	Ip   string
	time time.Duration
}

func main() {

	hosts, err := readHostsFromFile("/usr/local/gitcode/ss.txt")

	if err != nil {
		fmt.Println("Error reading hosts file:", err)
		return
	}

	ch := make(chan hostResult)
	var ipTimes []ipTime

	for _, host := range hosts {
		go func(h string) {
			delay, err := pingHost(h)
			ch <- hostResult{host: h, delay: delay, err: err}
		}(host)
	}
	for range hosts {
		result := <-ch
		if result.err != nil {
			// fmt.Printf("%s: Error: %v\n", result.host, result.err)
		} else {
			// fmt.Printf("%s: Delay: %s\n", result.host, result.delay)
			ipTimes = append(ipTimes, ipTime{Ip: result.host, time: result.delay})
		}
	}

	sort.Slice(ipTimes, func(i, j int) bool {
		return ipTimes[i].time < ipTimes[j].time
	})

	for _, ipTime := range ipTimes {
		fmt.Printf("%s took %v dial \n", ipTime.Ip, ipTime.time)
	}

}

func readHostsFromFile(filename string) ([]string, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	var hosts []string
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		u, err := url.Parse(scanner.Text())
		if err != nil {
			fmt.Print("wrong link \n", err)
			continue
		}
		domain := u.Hostname()
		ip := net.ParseIP(domain)
		if ip != nil {
			fmt.Printf("direct ip = %s \n", ip.String())
		} else {
			hosts = append(hosts, u.Host)
		}
	}
	if err := scanner.Err(); err != nil {
		return nil, err
	}

	return hosts, nil
}

func pingHost(host string) (time.Duration, error) {
	// ip, err := net.ResolveIPAddr("ip", host)
	// if err != nil {
	// 	return 0, err
	// }

	// conn, err := net.DialIP("ip4:icmp", nil, ip)
	// if err != nil {
	// 	return 0, err
	// }
	// defer conn.Close()
	start := time.Now()
	conn, err := net.DialTimeout("tcp", host, 2*time.Second)
	if err != nil {
		return 0, err
	}
	defer conn.Close()

	// _, err = conn.Write([]byte("HELLO"))
	// if err != nil {
	// 	return 0, err
	// }

	// reply := make([]byte, 1024)
	// err = conn.SetReadDeadline(time.Now().Add(time.Second))
	// if err != nil {
	// 	return 0, err
	// }

	// _, err = conn.Read(reply)
	// if err != nil {
	// 	return 0, err
	// }

	return time.Since(start), nil
}
