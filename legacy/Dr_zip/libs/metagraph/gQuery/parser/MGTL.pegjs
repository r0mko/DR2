{
  var nodeScopes = []
  var edgeScopes = []
  function checkType(typeName) {
    if (typeof isRegistered == "function")
      return isRegistered(typeName)
    return /[A-Z]/.test(typeName[0])
  }

  function checkReduceFn(funcName) {
    if (typeof findReduceFn == "function")
      return findReduceFn(funcName)
    return ["one", "last", "first"].indexOf(funcName) >= 0
  }

  function isNodeScope(id) {
    return nodeScopes.indexOf(id) >= 0
  }

  function isEdgeScope(id) {
    return edgeScopes.indexOf(id) >= 0
  }

  function defineEdgeScope(id) {
    if (!isEdgeScope(id))
      edgeScopes.push(id)
  }

  function defineNodeScope(id) {
    if (!isNodeScope(id))
      nodeScopes.push(id)
  }



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

  function flatten(arr) {
    return arr.reduce(function (flat, toFlatten) {
      return flat.concat(Array.isArray(toFlatten) ? flatten(toFlatten) : toFlatten);
    }, []);
  }

  // function buildCapture()
}

start
  = __ e: Query { return e.fixMeta() }

TrueToken       = "true"       !IdentifierPart
NullToken       = "null"       !IdentifierPart
FalseToken      = "false"      !IdentifierPart
InstanceofToken = "instanceof" !IdentifierPart
InToken         = "in"         !IdentifierPart


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
  = t: IdentifierName &{ return checkType(t); } { return t  }

PrimaryExpression
  = t: ObjectType { return { type: "ObjectType", name: buildTokenMeta(peg$currPos, t, "TypeName") }  }
  / id: ScopeId { return { type: "ScopeId", scope: id } }
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
  = CallExpression
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
  = lhs: LeftHandSideExpression __ "=" !"=" __ rhs: AssignmentExpression
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
  = st: StartClause __ e: Path {
    e.stages.unshift(st)
    return {
      type: "RootQuery",
      path: e
    }
  }

QueryOnNodes
  = st: (NodeClause / NodeScope) __ p: PathN {
    return {
      type: "Query",
      path: flatten([st, p])
    }
  }

QueryOnEdges
  = st: (EdgeClause / EdgeScope) __ p: PathE {
    return {
      type: "Query",
      path: flatten([st, p])
    }
  }

StartClause
  = StartScope
  / c: NodeClause &(__ PathN) { return c; }
  / c: EdgeClause &(__ PathE) { return c; }

StartScope
  = NodeScope 
  / EdgeScope
  / DefaultScope

NodeScope
  = id: NodeScopeId { return { type: "Scope", entity: "Node", name: id } }
  / s: ScopeClause &( __ PathN) { defineNodeScope(s);  return { type: "StartScope", entity: "Node", name: text() } }

EdgeScope
  = id: EdgeScopeId { return { type: "Scope", entity: "Node", name: id } }
  / s: ScopeClause &( __ PathE) { defineEdgeScope(s); return { type: "StartScope", entity: "Edge", name: text() } }

NodeScopeId
  = id: Identifier &{ return isNodeScope(id)} { return buildTokenMeta(peg$currPos, id, "ScopeId") }

EdgeScopeId
  = id: Identifier &{ return isEdgeScope(id)} { return buildTokenMeta(peg$currPos, id, "ScopeId") }

Path
  = PathEE
  / PathNE
  / PathEN
  / PathNN

PathE
  = PathEE
  / PathEN

PathN
  = PathNN
  / PathNE

PathNN
  = h: (s: StageNN __ { return s; })+ { return { type: "Path", in: "Node", out: "Node", stages: flatten(h) } }

PathEN
  = l: StageEN __ e: PathNN? { return { type: "Path", in: "Edge", out: "Node", stages: e ? flatten([l, e.stages]) : [l] } }

PathEE
  = l: PathEN? __ e: StageNE { return { type: "Path", in: "Edge", out: "Edge", stages: l ? flatten([l.stages, e]) : [e] } }

PathNE
  = l: PathNN? __ e: StageNE { return { type: "Path", in: "Node", out: "Edge", stages: l ? flatten([l.stages, e]) : [e] } }

StageEN
  = s1: TraversalEN __ s2: NodeClause { return [s1, s2] }

StageNE
  = s1: TraversalNE __ s2: EdgeClause !( __ TraversalEN) { return [s1, s2] }

StageNN
  = s1: TraversalNN __ s2: NodeClause { return [s1, s2] }
  / s1: TraversalNE __ s2: EdgeClause __ s3: TraversalEN __ s4: NodeClause { return [s1, s2, s3, s4] }

TraversalNN 
  = NodeHop
  / ChildrenTraversal
  / ParentTraversal

Clause
  = NodeClause
  / EdgeClause
  / ScopeClause

NodeClause "Node clause"
  = l: QueryLabel q: BracketedQueryOnNodes {
    return {
      type: "NamedQuery",
      label: l,
      query: q
    }
  }
  / st: NodeStatement { return st}

BracketedQueryOnNodes
  = "(" __ q: QueryOnNodes __ ")" { return q }

BracketedQueryOnEdges
  = "(" __ q: QueryOnEdges __ ")" { return q }

EdgeClause "Edge clause"
  = l: QueryLabel q: BracketedQueryOnEdges {
    return {
      type: "NamedQuery",
      label: l,
      query: q
    }
  }
  / st: EdgeStatement { return st }

Traversal
  = NodeHop
  / TraversalNE
  / TraversalEN
  / ChildrenTraversal 
  / ParentTraversal
  / NullTraversal
  
TraversalNE  "node -> edge[] traversal"
  = d: DirectionToken { return { type: "Traversal", direction: "NE" } }

TraversalEN "edge -> node traversal" 
  =  d: DirectionToken  { return { type: "Traversal", direction: "EN" } }

ChildrenTraversal "node children traversal"
  = "/" !(__ EdgeClause) { return { type: "ChildrenTraversal"} }

ParentTraversal "node parent traversal"
  = &(NodeClause __) "^" { return { type: "ParentTraversal" } }

NodeHop
  = d: HopToken { return { type: "NodeHop", direction: d } }

DirectionToken
  = $(">" !">") / $("<" !"<")

HopToken
  = ">>" / "<<"

NullTraversal
  = "-" { return { type: "NullTraversal"} }

ScopeClause "Scope clause"
  = id: Identifier { return id }

DefaultScope
  = "$" { 
    return {
      type: "DefaultScope",
      token: buildTokenMeta(peg$currPos, text(), "ScopeId") }
    }
     
Label
  = "/" _ id: LabelDef _ "/" &(__ EdgeFilter) { defineEdgeScope(id.text); return { type: "ScopeLabel", entity: "Edge", value: id } }
  / "/" _ id: LabelDef _ "/" &(__ NodeFilter) { defineNodeScope(id.text); return { type: "ScopeLabel", entity: "Node", value: id } }

QueryLabel
  = "/" _ id: LabelDef _ "/" &(__ BracketedQueryOnEdges) { defineEdgeScope(id.text); return { type: "ScopeLabel", entity: "Edge", value: id } }
  / "/" _ id: LabelDef _ "/" &(__ BracketedQueryOnNodes) { defineNodeScope(id.text); return { type: "ScopeLabel", entity: "Node", value: id } }

LabelDef
  = id: IdentifierName { return buildTokenMeta(peg$currPos, id, "ScopeId") }

NodeFilter
  = "(" __ l: LogicalExpression __ ")" { return l }

EdgeFilter
  = "[" __ l: LogicalExpression __ "]"  { return l }

NodeStatement 
  = l: Label? _ f: NodeFilter __ p: PostFilterActions?
  {
    return {
      type: "NodeStatement",
      label: l,
      filter: f,
      postFilter: p ? flatten(p) : null
    }
  }

EdgeStatement
  = l: Label? f: EdgeFilter __ p: PostFilterActions?
  {
    return {
      type: "EdgeStatement",
      label: l,
      filter: f,
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

ScopeId
  = EdgeScopeId
  / NodeScopeId

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
  / RunQueryById
  / ScopeRoutine
  / ScriptRoutine

ScopeRoutine
  = id: NodeScopeId __ "." __ "query" __ q: ExecNodeQuery {
    q.start = id
    return q
  }
  / id: EdgeScopeId __ "." __ "query" __ q: ExecEdgeQuery {
    q.start = id
    return q
  }

RunQueryById
  = q: (DefaultScope/ScopeId) __ "(" __ s: (DefaultScope/ScopeId) __ ")" {
    return {
      type: "ExecQueryById",
      query: q,
      scope: s
    }
  }

ExecNodeQuery
  =  "(" __ q: (QueryOnNodes / NodeScopeId / DefaultScope) __ ")" {
    return {
      type: "ExecQueryRoutine",
      query: q
    }
  }

ExecEdgeQuery
  = "(" __ q: (QueryOnEdges / EdgeScopeId / DefaultScope) __ ")" {
    return {
      type: "ExecQueryRoutine",
      query: q
    }
  }

ScriptRoutine
  = e: AssignmentExpression {
    return {
      type: "ScriptRoutine",
      script: e
    }
  }

CaptureRoutine "CaptureRoutine"
  = SingleCapture
  / SubstitutedCaptureList

SingleCapture
  =  "+" id:IdentifierName {
    return {
      type: "CaptureRoutine",
      key: id,
      expression: id
    }
  }

SubstitutedCaptureList
  = "+" _ "{" __ hd: SubstitutedCapture rest: (__ "," __ t: SubstitutedCapture { return t })*  __ "}"
  {
    return [hd].concat(rest)
  }

SubstitutedCapture
  = n: Identifier __ ":" __ e: AssignmentExpression { return { type: "CaptureRoutine", key: n, expression: e } }

