 

let x = 0;

 

const source = "let x: string  = 'string'";

function fib(x:number)
{
    if (x <= 1) 
    {
        return 1;
    }
    return fib(x-1) + x;
}

let result = fib(90);
 
console.log(result)