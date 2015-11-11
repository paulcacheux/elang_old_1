mod io {
    extern func print(elem : int) -> void;
    extern func read() -> int;

    mod test {
        func test1() -> int {
            return read();
        }
        mod test2 {
            func depth() -> int {
                return 42;
            }
        }
        mod test3 {
            func other_mod() -> *char {
                let a : int = test2::depth();
                return "test";
            }           
        }
    }
}
