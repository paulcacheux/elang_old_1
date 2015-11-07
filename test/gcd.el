# a program to perform euclid's algorithm to compute gcd

import io

func gcd(u : int, v : int) -> int {
    if v == 0 {
        return u;
    } else {
        return gcd(v, u - u % v); # u / v * v == u % v
    }
}

func main() -> void {
    let x : int = input();
    let y : int = input();
    io::print(gcd(x, y));
}
