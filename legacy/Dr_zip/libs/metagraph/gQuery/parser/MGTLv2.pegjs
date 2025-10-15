{
  var edgeScopes = []
  var nodeScopes = []

  function checkType(typeName) {
    if (typeof isRegistered == "function")
      return isRegistered(typeName)
    return /[A-Z]/.test(typeName[0])
  }

  function isContextField(id) {
    if (typeof isContextProperty == "function")
      return isContextProperty(id)
    return ["result", "out", "in"].indexOf(id) >= 0
  }

  function checkReduceFn(funcName) {
    if (typeof findReduceFn == "function")
      return findReduceFn(funcName)
    return ["one", "last", "first"].indexOf(funcName) >= 0
  }

  function defineNodeScope(id) {
    if (!isNodeScope(id))
      nodeScopes.push(id)
  }

  function ctx(name) {
    var _head = {
      type: "Head"
    }
    var _tail = {
      type: "Tail",
      prev: _head
    }
    _head.next = _tail
    this.path = [ _head, _tail ]
    this.name = name ? name : ""

    this.tail = function() { return _tail }
    this.last = function() { return _tail.prev }
    
    this.add = function(type, object) {
      var i = this.path.length - 1
      var o = { type: type, value: object, next: _tail, prev: _tail.prev }
      o.next = _tail
      o.prev = this.path[i - 1]
      this.path[i - 1].next = o
      _tail.prev = o
      this.path.insert(i, o)
    }
    return this
  } 

  function track(name) {
    var _path = []
    this.path = function() { return _path }
    this.name = name ? name : ""
    this.add = function(object) {
      _path.push(object)
    }
  }

  function Query() {
    var _stack = [new track("root")]

    this.current = function() { return _stack[_stack.length - 1] }
    this.expand = function(name) {
      var t = new track(name)
      t.type = "Path"
      this.current().add(t.path())
      _stack.push(t)
    }

    this.reduce = function() {
      if (_stack.length > 1) {
        _stack.pop()
        return
      } 
      var rt = _stack.pop()
      var outer = new track()
      if ("negDepth" in rt)
        outer.negDepth = rt.negDepth - 1
      else
        outer.negDepth = -1
      outer.add(rt)
      _stack.push(outer)
    }

    this.add = function(object) {
      this.current().add(object)
    }

    this.path = function() { return _stack[0].path() }
  }


  var mainPath = new ctx()
  var namedPaths = {}
  var currentPath = mainPath

  function addSubPath(label) {
    var ret = new ctx(label);
    if (label) 
      addId(label, ret)
    add("Path", { path: ret, parent: this.currentPath })
    currentPath = ret
    console.log("adding subpath", this.currentPath)
    return ret
  }

  function endSubPath() {
    if ('parent' in currentPath) 
      currentPath = currentPath.parent
    return currentPath
  }

  function add(type, object) {
    currentPath.add(type, object)
  }

  function addId(label, path) {
    if (label in namedPaths) {
      throw new Error("duplicate context id <"+ label + ">!")
      return null
    }
    console.log("Added context id", label)
    namedPaths[label] = path
  }

  function isContextId(id) { return id in namedPaths }

  Object.defineProperty(Array.prototype, "insert", {
    enumerable: false,
    writable: true,
    value: function (index, item) { this.splice(index, 0, item) }
  })

  Object.defineProperty(Object.prototype, "fixMeta", {
    enumerable: false,
    writable: true,
    value: function(root) {
      var iAmRoot = false
      if (!root) {
        iAmRoot = true
        root = []
      }
      for (var key in this) {
        var v = this[key];
        if (v instanceof Object && !(v instanceof Function)) {
          if (v.hasOwnProperty("text") && v.hasOwnProperty("_token")) {
            var tm = { meta: v._token, key: key, type: this.type }
            root.push(tm)
            this[key] = v.text
          } else v.fixMeta(root)
        }
      }
      if (iAmRoot)
        this.metadata = root
      return this
    }
  })

  function buildPath(obj) {
    if(!obj instanceof Object || !"type" in obj || obj.type != "Path")
      return
    obj.steps.forEach(function() {})
  }

  function buildList(obj, arr) {
    if (obj instanceof Array) {
      obj.forEach(function(e) {
        buildList(e, arr)
      })
    } else {
      arr.push(obj)
    }
  }

  function buildTokenMeta(pos, token, type, extra) {
    return {
      text: token,
      _token: {
        type: type,
        pos: pos - 1,
        count: token.length,
        extra: extra,
        token: token
      }
    }
  }

  function has(obj, prop) {
      return Object.prototype.hasOwnProperty.call(obj, prop);
  }

  function flatten(arr) {
    return arr.reduce(function (flat, toFlatten) {
      return flat.concat(Array.isArray(toFlatten) ? flatten(toFlatten) : toFlatten);
    }, []);
  }
}

start
  = __ e: Query __ { console.log(mainPath); return e.fixMeta() }

TrueToken       = "true"       !IdentifierPart
NullToken       = "null"       !IdentifierPart
FalseToken      = "false"      !IdentifierPart
InstanceofToken = "instanceof" !IdentifierPart
InToken         = "in"         !IdentifierPart
ReturnToken     = "return"     !IdentifierPart
ThisToken       = "this"       !IdentifierPart


Identifier
  = IdentifierName


DollarIdentifier
  = "$" chars:IdentifierPart* { return chars.join("") }

IdentifierName "identifier"
  = first:IdentifierStart rest:IdentifierPart*
  { return first + rest.join("") }

IdentifierStart
  = Letter
  / "_"

IdentifierPart
  = IdentifierStart
  / DecimalDigit

Letter
  = [A-Za-z]

SourceCharacter
  = .

Comment "comment"
  = MultiLineComment
  / SingleLineComment

MultiLineComment
  = "/*" (!"*/" SourceCharacter)* "*/"

MultiLineCommentNoLineTerminator
  = "/*" (!("*/" / LineTerminator) SourceCharacter)* "*/"

SingleLineComment
  = "//" (!LineTerminator SourceCharacter)*

__
  = z:(WhiteSpace / LineTerminatorSequence / Comment)* { return "" }

_
  = (WhiteSpace / MultiLineCommentNoLineTerminator)* { return "" }

WhiteSpace "whitespace"
  = "\t"
  / "\n"
  / "\f"
  / " "

LineTerminator
  = [\n\r\u2028\u2029]

LineTerminatorSequence "end of line"
  = "\n" / "\r\n" / "\r"/ "\u2028" / "\u2029"

DecimalDigit
  = [0-9]

NonZeroDigit
  = [1-9]

DecimalIntegerLiteral
  = "0"
  / NonZeroDigit DecimalDigit*

NumericLiteral "number"
  = DecimalLiteral !(IdentifierStart / DecimalDigit)

Literal
  = NullLiteral { return buildTokenMeta(peg$currPos, text(), "NullLiteral") }
  / BooleanLiteral { return buildTokenMeta(peg$currPos, text(), "BooleanLiteral")  }
  / NumericLiteral { return buildTokenMeta(peg$currPos, text(), "NumericLiteral") }
  / StringLiteral { return buildTokenMeta(peg$currPos, text(), "StringLiteral") }

BooleanLiteral
  = TrueToken
  / FalseToken

NullLiteral
  = NullToken

DecimalLiteral
  = DecimalIntegerLiteral "." DecimalDigit* ExponentPart? { return text() }
  / "." DecimalDigit+ ExponentPart? { return text() }
  / DecimalIntegerLiteral ExponentPart?  { return text() }

ExponentPart
  = ExponentIndicator SignedInteger

ExponentIndicator
  = "e"i

SignedInteger
  = [+-]? DecimalDigit+

StringLiteral "string"
  = '"' DoubleStringCharacter* '"'
  / "'" SingleStringCharacter* "'"

DoubleStringCharacter
  = !('"' / "\\" / LineTerminator) SourceCharacter
  / "\\" EscapeSequence
  / LineContinuation

SingleStringCharacter
  = !("'" / "\\" / LineTerminator) SourceCharacter
  / "\\" EscapeSequence
  / LineContinuation

LineContinuation
  = "\\" LineTerminatorSequence

EscapeSequence
  = CharacterEscapeSequence

CharacterEscapeSequence
  = SingleEscapeCharacter
  / NonEscapeCharacter

SingleEscapeCharacter
  = "'"
  / '"'
  / "\\"
  / "b"
  / "f"
  / "n"
  / "r"
  / "t"
  / "v"     // IE does not recognize "\v".

EscapeCharacter
  = SingleEscapeCharacter
  / DecimalDigit
  / "x"
  / "u"

NonEscapeCharacter
  = !(EscapeCharacter / LineTerminator) SourceCharacter

UnaryExpression
  = PostfixExpression
  / op: UnaryOperator __ e: UnaryExpression { return { type: "UnaryExpression", operator: op, expression: e } }


UnaryOperator
  = "++"
  / "--"
  / $("+" !"=")
  / $("-" !"=")
  / "~"
  / "!"

PostfixOperator
  = "++"
  / "--"

ObjectType
  = t: IdentifierName &{ return checkType(t); } { return t }

PrimaryExpression
  = t: ObjectType { return { type: "ObjectType", name: buildTokenMeta(peg$currPos, t, "TypeName") }  }
  / AnyContext
  / Context
  / Identifier { return { type: "Identifier", value: buildTokenMeta(peg$currPos, text(), "Identifier") } }
  / t: Literal { return { type: "Literal", value: t } }
  / t: ArrayLiteral { return { type: "ArrayLiteral", value: t } }
  / t: ObjectLiteral { return { type: "ObjectLiteral", value: t } }
  / "(" __ e:Expression __ ")" { return { type: "BracketedExpression", value: e } }

ArrayLiteral
  = "[" __ (Elision __)? "]"  { return null }
  / "[" __ l: ElementList __ "]"  { return l }
  / "[" __ l: ElementList __ "," __ (Elision __)? "]" { return l }

ElementList
  = head: (e: (Elision __)? v: AssignmentExpression { return e ? [e, v] : [v] }) rest: (__ "," __ e: (Elision __)? v: AssignmentExpression { return e ? [e, v] : [v]})* {
    var ret = head
    rest.forEach(function(el) {
      el.forEach(function(el2) {
          ret.push(el2)
        })
    })
    return ret
  }

Elision
  = "," (__ ",")*

PostfixExpression
  = lhs: LeftHandSideExpression _ op: PostfixOperator { return { type: "PostfixExpression", lhs: lhs, operator: op } }
  / lhs: LeftHandSideExpression { return lhs }

MemberExpression
  = head:PrimaryExpression
  rest: (
    __ e: SquareBracketedExpression {return e; }/ __ e: DotAccess { return e; }
  )*
  {
    return rest.length ? { type: "MemberExpression", head: head, extra: rest } : head
  }

SquareBracketedExpression
  = "[" __ e:Expression __ "]"
  { return { type: "SquareBracketedExpression", value: e } }

DotAccess
  = "." __ id: IdentifierName
  { return { type: "DotAccess", value: id } }

ObjectLiteral
  = "{" __ "}" { return null }
  / "{" __ v:PropertyNameAndValueList __ "}" { return v }
  / "{" __ v:PropertyNameAndValueList __ "," __ "}" { return v }

PropertyNameAndValueList
  = hd: PropertyAssignment rest: (__ "," __ t: PropertyAssignment { return t })* { return [hd].concat(rest) }

PropertyAssignment
  = n: PropertyName __ ":" __ e: AssignmentExpression { return { type: "PropertyAssignment", name: n, value: e } }

PropertyName
  = IdentifierName
  / StringLiteral
  / NumericLiteral

LeftHandSideExpression
  = EvalQueryExpression
  / CallExpression
  / MemberExpression

CallExpression
  = head:(e: MemberExpression __ args: Arguments { return { expr: e, args: args } })
  rest:(
    __ e: Arguments { return e; } /
    __ e: SquareBracketedExpression { return e; } /
    __ e: DotAccess { return e; }
  )*
  {
    return {
      type: "CallExpression",
      expression: head.expr,
      arguments: head.args,
      extra: rest
    }
  }

Arguments
  = "(" __ a:(args: ArgumentList __ { return args })? ")" { return { type: "Arguments", list: a } }

ArgumentList
  = head:AssignmentExpression rest:(__ "," __ e: AssignmentExpression { return e } )*
  { return [head].concat(rest) }

Expression
  = head:AssignmentExpression rest:(__ "," __ e: AssignmentExpression { return e })*
  { return rest.length ? [head].concat(rest) : head }

AssignmentExpression
  = BracketedPath 
  / lhs: LeftHandSideExpression __ "=" !"=" __ rhs: AssignmentExpression
  { return { type: "AssignmentExpression", operator: "=", lhs: lhs, rhs: rhs} }
  / lhs: LeftHandSideExpression __ op: AssignmentOperator __ rhs: AssignmentExpression
  { return { type: "AssignmentExpression", operator: op, lhs: lhs, rhs: rhs} }
  / ConditionalExpression

AssignmentOperator
  = "*="
  / "/="
  / "%="
  / "+="
  / "-="
  / "<<="
  / ">>="
  / ">>>="
  / "&="
  / "^="
  / "|="

ConditionalExpression
  = c: LogicalExpression __ "?" __ t: AssignmentExpression __ ":" __ f: AssignmentExpression { return { type: "TernaryOperator", condition: c, positive: t, negative: f } }
  / e: LogicalExpression { return e.type ? e : { type: "ConditionalExpression", expression: e } }

LogicalExpression
  = head:EqualityExpression rest:(__ op: LogicalOperator __ e: EqualityExpression { return { operator: op, expr: e } } )*
  { return rest.length ? { type: "LogicalExpression", lhs: head, rhs: rest} : head }

LogicalOperator
  = BitwiseANDOperator / BitwiseXOROperator / BitwiseOROperator / LogicalANDOperator / LogicalOROperator

BitwiseANDOperator
  = $("&" ![&=])

BitwiseXOROperator
  = $("^" !"=")

BitwiseOROperator
  = $("|" ![|=])

LogicalANDOperator
  = "&&"

LogicalOROperator
  = "||"

EqualityExpression
  = head:RelationalExpression rest:(__ op: EqualityOperator __ e: RelationalExpression { return { operator: op, expr: e } } )*
  { return rest.length ? { type: "EqualityExpression", lhs: head, rhs: rest} : head }

EqualityOperator
  = "==="
  / "!=="
  / "=="
  / "!="

MultiplicativeExpression
  = head:UnaryExpression rest:(__ op: MultiplicativeOperator __ e: UnaryExpression { return { operator: op, expr: e } } )*
  { return rest.length ? { type: "MultiplicativeExpression", lhs: head, rhs: rest} : head }

MultiplicativeOperator
  = $("*" !"=")
  / $("/" !"=")
  / $("%" !"=")

AdditiveExpression
  = head:MultiplicativeExpression rest:(__ op: AdditiveOperator __ e: MultiplicativeExpression { return { operator: op, expr: e } } )*
  { return rest.length ? { type: "AdditiveExpression", lhs: head, rhs: rest} : head }

AdditiveOperator
  = $("+" ![+=])
  / $("-" ![-=])

ShiftExpression
  = head:AdditiveExpression rest:(__ op:ShiftOperator __ e: AdditiveExpression { return { operator: op, expr: e } })*
  { return rest.length ? { type: "ShiftExpression", lhs: head, rhs: rest} : head }

ShiftOperator
  = $("<<"  !"=")
  / $(">>>" !"=")
  / $(">>"  !"=")

RelationalExpression
  = head:ShiftExpression rest:(__ op: RelationalOperator __ e:ShiftExpression { return { operator: op, expr: e } } )*
  { return rest.length ? { type: "RelationalExpression", lhs: head, rhs: rest} : head }

RelationalOperator
  = "<="
  / ">="
  / $("<" !"<")
  / $(">" !">")
  / $InstanceofToken
  / $InToken

Query
  = Path
  / QueryObject

Path
  = st: StartClause p: (__ t: Traversal t2: Traversal? __ c: Clause { return t2 ? [t, t2, c] : [t, c] })* {
    var steps = []
    if (st.type !== "CurrentContext")
      steps.push(st)
    steps.push(p)
    return {
      type: "Path",
      steps: flatten(steps)
    }
  }

QueryObject
  = "{" __ s: (o:PathDeclaration __ { return o })* __ ret:ReturnStatement __ "}" { 
    return {
      type: "QueryObject",
      decl: s,
      ret: ret
    }
  }

NamedContext
  = LabeledPath
  / PathDeclaration

PathDeclaration
  = id: LabelDef _ ":" _ q:Path { 
    return {
      type: "PathDeclaration",
      path: q,
      id: id
     } 
   }

ReturnStatement "QueryObject return statement"
  = ReturnToken __ s: Path {
    return s
  }

LabeledPath
  = id: QueryLabel q: BracketedPath { return { id: id, path: q } }

BracketedPath
  = "(" __ q: Path __ ")" { return q }

StartClause
  = Clause
  / AnyContext

AnyContext
  = id: ContextId !(__ "(") { return { type: "ContextId", context: id } }
  / CurrentContext

Clause
  = NodeClause
  / EdgeClause
  / EvalQueryStatement

EvalQuery
  = id: ContextId __ "(" _ arg: AnyContext? _ ")" { 
    return {
      type: "EvalQuery",
      query: id,
      contextId: arg
    }
  } 
  / "$" a: BracketedPath {
    return {
      type: "InlineQuery",
      query: a
    }
  }

EvalQueryExpression
  = e: EvalQuery __ "." __ f: ContextField { 
    return {
      type: "EvalQueryExpression",
      query: e,
      field: f
    }
  }

ContextField
  = id:IdentifierName &{ return isContextField(id) } { return id }

NodeClause "Node clause"
  = LabeledPath
  / st: NodeStatement { return st }

EdgeClause "Edge clause"
  = l: QueryLabel q: BracketedPath {
    return {
      type: "NamedPath",
      label: l,
      path: q
    }
  }
  / st: EdgeStatement { return st }

Traversal
  = TraversalNE { return { type: "Traversal", direction: "NE" } }
  / TraversalEN { return { type: "Traversal", direction: "EN" } }
  / ChildrenTraversal { return { type: "Traversal", direction: "Children"} }
  / ParentTraversal { return { type: "Traversal", direction: "Parent" } }
  / NullTraversal { return { type: "NullTraversal" } }
  / Recursion { return { type: "Traversal", direction: "Recursion" } }


TraversalNE  "node -> edge[] traversal"
  = d: DirectionToken &(TraversalEN) 
  / d: DirectionToken &( __ EdgeClause) 

TraversalEN "edge -> node traversal" 
  = d: DirectionToken &(__ NodeClause) 
  / &(TraversalNE) d: DirectionToken

ChildrenTraversal "node children traversal"
  = "/" 

ParentTraversal "node parent traversal"
  = "^"

DirectionToken
  = ">" / "<"

NullTraversal "null parent traversal"
  = ("-" !"-") 
ScopeClause "Scope clause"
  = id: Identifier { return id }

Recursion
  = ">>>" 


CurrentContext
  = $ThisToken { 
    return {
      type: "CurrentContext",
      token: buildTokenMeta(peg$currPos, text(), "ContextId") 
    }
  }
  

StatementLabel
  = "/" _ id: LabelDef _ "/" &(__ EdgeFilter) { return id; }
  / "/" _ id: LabelDef _ "/" &(__ NodeFilter) { return id; }

QueryLabel
  = "/" _ id: LabelDef _ "/" &(__ BracketedPath) { return id }

ContextId "Context Id"
  = id: IdentifierName &{ return isContextId(id) } { return id }

LabelDef
  = id: IdentifierName { addId(id); return buildTokenMeta(peg$currPos, id, "ContextId") }

NodeFilter
  = "(" __ l: LogicalExpression __ ")" { return l }

EdgeFilter
  = "[" __ l: LogicalExpression __ "]"  { return l }

NodeStatement 
  = l: StatementLabel? _ f: NodeFilter __ p: PostFilterActions?
  {
    return {
      type: "NodeStatement",
      label: l,
      filter: f,
      postFilter: p ? flatten(p) : null
    }
  }

EdgeStatement
  = l: StatementLabel? f: EdgeFilter __ p: PostFilterActions?
  {
    return {
      type: "EdgeStatement",
      label: l,
      filter: f,
      postFilter: p ? flatten(p) : null
    }
  }

EvalQueryStatement
  = c: EvalQuery __ p: PostFilterActions?
  {
    return {
      type: "EvalQueryStatement",
      queryId: c.queryId,
      contextId: c.contextId,
      postFilter: p ? flatten(p) : null
    }
  }

PostFilterActions
  = red: ReduceRoutine __ cond: ConditionalAction { return [red, cond]}
  / red: ReduceRoutine { return [red] }
  / cond: ConditionalAction { return [cond] }

ReduceRoutine
  = s: SquareBracketedExpression {
    return {
      type: "IndexReduce",
      value: s.value
    }
  }
  / ReduceFunction 

ReduceFunction
  = "." __ fn: ReduceFnName __ arg: Arguments {
    return {
      type: "ReduceFunction",
      name: fn,
      args: arg
    }
  }

ReduceFnName "reduce function name"
  = id: Identifier &{ return checkReduceFn(id) } { return id; }

DoStatement
  = "do" __  lst: RoutineList
  {
    return {
      type: "DoStatement",
      routines: lst.routines
    }
  }

ConditionalAction "Conditional action"
  = "?" __ succ: RoutineList __ ":" __ fail: RoutineList {
    return {
      type: "ConditionalAction",
      positive: succ,
      negative: fail
    }
  }
  / "." st: DoStatement { return st; }


RoutineList
  = "(" __ hd: Routine rest: (__ "," __ a: Routine { return a } )* __ ")"
  { 
    return {
      type: "RoutineList",
      routines: flatten([hd].concat(rest)) 
    }
  }
  
Routine
  = CaptureRoutine
  / ScriptRoutine


ScriptRoutine
  = e: AssignmentExpression {
    return {
      type: "ScriptRoutine",
      script: e
    }
  }

CaptureRoutine "CaptureRoutine"
  = ContextCapture
  / "+" c:SingleCapture { return c; }
  / "+" _ c:SubstitutedCaptureList { return c; }

ContextCapture
  = ctx:ContextId __ "+=" __ c: (SingleCapture / SubstitutedCaptureList) {
    return {
      type: "ContextCapture",
      context: ctx,
      capture: c
    }
  }

SingleCapture
  = id:IdentifierName {
    return {
      type: "CaptureRoutine",
      key: id,
      expression: id
    }
  }

SubstitutedCaptureList
  = "{" __ hd: SubstitutedCapture rest: (__ "," __ t: SubstitutedCapture { return t })*  __ "}"
  { return [hd].concat(rest) }

SubstitutedCapture
  = n: Identifier __ ":" __ e: AssignmentExpression { return { type: "CaptureRoutine", key: n, expression: e } }

