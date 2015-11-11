# a program to perform euclid's algorithm to compute gcd

mod io {
    func print(elem : int) -> void;
    func read() -> int;
}

func gcd(u : int, v : int) -> int {
    if v == 0 {
        return u;
    } else {
        return gcd(v, u - u % v);
    }
}

func main() -> void {
    let x = io::read();
    let y = io::read();
    io::print(gcd(x, y));
}
