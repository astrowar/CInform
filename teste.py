


class Kind:
    def __init__(self, name, basekind):
        self.name = name
        self.kind= basekind

    def __repr__(self):
        return "K:"+self.name


class Instance:
    def __init__(self, name, kind):
        self.name = name
        self.kind= kind
    def __repr__(self):
        if self.kind == None :
            return "I:"+self.name
        return "I:"+self.name+"("+self.kind.name+")"

class Interpreter:

    def __init__(self):
        self.kinds = []
        self.instances =[]
        self.kinds.append(Kind("kind", None))
        self.kinds.append(Kind("value", None))

        pass

    def getKind(self,name):
        for k in self.kinds:
            if k.name.lower() == name.lower() : return k
        return None

    def isKind(self,name):
        for k in self.kinds:
            if k.name.lower() == name.lower() : return True
        return False

    def isValue(self,name):
        if name.lower() == "value" :
            return True
        kb = self.getKind(name)
        if (kb == None):
            return False
        return self.isValue( kb.kind.name )



    def addKind(self,k,base):
        if self.isKind(base ):
           self.kinds.append(  Kind(k,self.getKind(base ) ) )
        else:
            raise Exception("Kind base Error")

    def addInstance(self,n,base):
        if self.isKind(base ):
           self.instances.append(  Instance(n,self.getKind(base ) ) )
        else:
            raise Exception("Kind base Error")

    def addEnum(self,n,base):
        if self.isValue(base ):
           self.instances.append(  Instance(n,self.getKind(base ) ) )
        else:
            raise Exception("Kind base Error")

class ParseCommand:
    def __init__(self,inter,_cmd):
        self.verb = ""
        self.args = ""
        self.interpreter = inter
        self.cmd = _cmd
        c = _cmd.split()
        print(c)
        if (len(c)) == 0:
            return
        self.verb = c[0]
        self.args = c[1:]

        if self.verb.lower() == "new":
            self.verb = c[0]+c[1]
            self.args = c[2:]

        if self.verb.lower() == "add":
            self.verb = c[0]+c[1]
            self.args = c[2:]

        self.verb= self.verb.lower()

    def execute(self):
        print(self.verb)
        if self.verb =="newkind":
           self.interpreter.addKind(self.args[0],self.args[1]  )
        if self.verb =="newinstance":
           self.interpreter.addInstance(self.args[0],self.args[1]  )
        if self.verb =="newvalue":
           self.interpreter.addEnum(self.args[0],self.args[1]  )


class ParseTree:
    def __init__(self,inner, idx =0):
        self.inner = inner+[]
        self.idx = idx

    def execute(self, interpreter ):
        for x in self.inner:
            if isinstance(x, str) :
                p = ParseCommand( interpreter , x)
                p.execute()
            if isinstance(x, ParseTree) :
                x.execute(interpreter)


    def contents(self):
        sidx = ' ' * self.idx
        yield sidx+"BEGIN"
        for x in self.inner :
            if isinstance(x, str) :
                yield sidx+ x
            if isinstance(x, ParseTree) :
                for xx in x.contents():
                    yield xx
        yield sidx + "END"

    def __repr__(self):
        x = list( self.contents())
        return str(x)



def getIdent(a):
        i=0
        while a[i] ==' ' : i =i+1
        return i


def getGroups(lines , ident = 0  ):
    remainder = lines+[]
    g = []
    while len(remainder) > 0 :
        a = remainder[0]
        idx = getIdent(a)
        a = a.strip()
        if idx > ident :
           Pinner ,remainder =  getGroups( remainder , idx )
           g.append( Pinner )
        if idx == ident:
           g.append( a)
           remainder = remainder[1:]
        if idx < ident:
           return ParseTree(g , ident ) , remainder
    return ParseTree(g , ident) , remainder


def parseTeste(lines):
    groups =  getGroups(lines)
    return groups




lns = open("tests\\teste_001.txt").readlines()
g , rem = parseTeste(lns)
print(g)
for x in g.contents():
    print(x)

inter = Interpreter()
g.execute(inter)

print(inter.kinds)
print(inter.instances)