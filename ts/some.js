var x = 0;
var source = "let x: string  = 'string'";
function fib(x) {
    if (x <= 1)
        return 1;
    return fib(x - 1) + x;
}
var result = fib(90);
console.log(result);
