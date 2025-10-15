{

	var seqIds = []
	var labels = []

	function checkType(typeName) {
		if (typeof isRegistered == "function")
			return isRegistered(typeName)
		return /[A-Z]/.test(typeName[0])
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
				extra: extra
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
	= __ e: Query __ { return e }

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
	/ UnaryOperator __ UnaryExpression

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

PrimaryExpression
	= Identifier { return { type: "Identifier", value: buildTokenMeta(peg$currPos, text(), "Identifier") } }
	/ id: DollarIdentifier { return { type: "DollarIdentifier", value: buildTokenMeta(peg$currPos, id, "DollarIdentifier") } }
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
	= s: Sequence
	{
		return {
			type: "Query",
			sequence: s.statements
		}.fixMeta()
	}

Sequence
	= hd: QueryElement __ s: (e: QueryElement __ { return e })*
	{
		return {
			type: "Sequence",
			statements: flatten([hd].concat(s))
		}
	}

QueryElement "query element"
	= NamedScopeBlock
	/ PathElement
	/ ScopeSelector

PathElement
	= NodeStatement
	/ EdgeStatement
	/ Traversal

ScopeSelector
	= "$" id: ScopeId {
		return {
			type: "ScopeSelector",
			scope: id
		}
	}
	/ "$" {
		return {
			type: "ScopeSelector",
			scope: null
		}
	}

Label
	= "/" _ id: LabelDef _ "/" &(__ FilterClauseList) { return { type: "Label", value: id } }

LabelDef
	= id: IdentifierName { return buildTokenMeta(peg$currPos, id, "Label") }

EdgeFilter
	= "[" __ l: FilterClauseList __ "]" { return l }

EdgeDummy
	= "[]" { return null }

FilterClauseList
	= head: FilterClause rest: (__ "," __ c: FilterClause { return c } )*
	{ return { type: "FilterClauseList", filters: [head].concat(rest) } }

EdgeStatement
	= l: Label? r: ReduceFn? f: EdgeFilter o: ConditionalAction? {
		return {
			type: "EdgeStatement",
			reduce: r,
			label: l,
			filters: f,
			routines: o
		}
	}
	/ l:Label? EdgeDummy st: DoStatement? {
		return {
			type: "EdgeDummyStatement",
			reduce: null,
			label: l,
			filters: null,
			routines: o
		}
	}



ReduceFn
	= "[" _ id:Identifier _ "]" &(NodeFilter/EdgeFilter) { return buildTokenMeta(peg$currPos, id, "ReduceFn") }

NodeFilter
	= "(" __ l: FilterClauseList __ ")"  { return l }

NodeStatement "$ statement"
	= l: Label? _ r: ReduceFn? f: NodeFilter __ o: ConditionalAction?
	{
		return {
			type: "NodeStatement",
			filters: f,
			label: l,
			reduce: r,
			routines: o
		}
	}

NamedScopeBlock
	= "{" __ s: NamedSequenceList __ "}" __ r: ScopeReduce? {
		return {
			type: "NamedScopeBlock",
			scopes: s,
			reduce: r
		}
	}

DoStatement
	= ".do" __  lst:RoutineList
	{
		return {
			type: "DoStatement",
			routines: lst
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
	/ st: DoStatement


NamedSequenceList
	= hd: NamedSequence rest: (__ "," __ s: NamedSequence { return s })* { return [hd].concat(rest); }

ClosureDef
	= id: IdentifierName

NamedSequence
	= id: PropertyName &{ return seqIds.indexOf(id) === -1 } __ ":" __ s: Sequence  {
		seqIds.push(id);
		return {
			type: "NamedSequence",
			name: id,
			sequence: s.statements
		}
	}

ScopeReduce
	= sr: SingularScopeReduce { return { type: "SingularScopeReduce", closure: sr } }
	/ r: MultiScopeReduce { return { type: "MultiScopeReduce", collection: r } }

SingularScopeReduce
	= "." __ id: ScopeId { return id; }
	/ "[" __ id: ScopeId __ "]" { return id; }

MultiScopeReduce
	= "[" __ id: ScopeIdList __ "]" { return id; }

ScopeIdList
	= hd: ScopeId rest: (__ id: ScopeId { return id; })* { return [hd].concat(rest); }

ScopeId
	= id: IdentifierName &{ return seqIds.indexOf(id) >= 0 } { return buildTokenMeta(peg$currPos, id, "ScopeId"); }

FilterClause
	= t:TypeClause
	{
		return {
			type: "TypeFilter",
			types: t
		}
	}
	/ e: ExpressionFilter
	{
		return {
			type: "ExpressionFilter",
			expression: e
		}
	}

TypeClause
	= hd:TypeUnary rest:(__ "|" __ t: TypeUnary { return t })*
	{ return [hd].concat(rest); }

TypeUnary
	= $("!" GraphType)
	/ GraphType

GraphType
	= ltype:IdentifierName &{ return checkType(ltype); } { return buildTokenMeta(peg$currPos, ltype, "GraphEntity") }

RoutineList
	= "(" __ hd: Routine rest: (__ "," __ a: Routine { return a } )* __ ")"
	{ return flatten([hd].concat(rest)) }

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

Traversal
	= NodeEdgesHop
	/ EdgeCapNodeHop
	/ ChildNodesHop
	/ ParentNodeHop
	/ SkipHop


EdgeCapNodeHop
	= !WhiteSpace ">" &WhiteSpace { return { type: "EdgeCapNodeHop", hop: "end", token: buildTokenMeta(peg$currPos, text(), "TraversalToken") } }
	/ !WhiteSpace "<" &WhiteSpace { return { type: "EdgeCapNodeHop", hop: "start", token: buildTokenMeta(peg$currPos, text(), "TraversalToken") } }

NodeEdgesHop
	= ">" !WhiteSpace { return { type: "NodeEdgesHop", hop: "out", token: buildTokenMeta(peg$currPos, text(), "TraversalToken") } }
	/ "<" !WhiteSpace { return { type: "NodeEdgesHop", hop: "in", token: buildTokenMeta(peg$currPos, text(), "TraversalToken") } }

ChildNodesHop
	= "/" { return { type: "ChildNodesHop", token: buildTokenMeta(peg$currPos, text(), "TraversalToken") } }

ParentNodeHop
	= "^" { return { type: "ParentNodeHop", token: buildTokenMeta(peg$currPos, text(), "TraversalToken") } }

SkipHop
	= "-" { return { type: "SkipHop", token: buildTokenMeta(peg$currPos, text(), "TraversalToken") } }

ExpressionFilter
	= e:LogicalExpression
	{ return buildTokenMeta(peg$currPos, e, "ExpressionFilter")	}

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

