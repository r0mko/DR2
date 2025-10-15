import QtQuick 2.0
import MetaGraph 1.0

Builder {
    id: root

    property var scopes

    function emitProgram(obj) {
        for (var i in obj.paths) {
            console.log("Building path", i)
            emitPath(obj.paths[i])
            addPathDefinition(currentPath, obj.paths[i].localId)
            endPath()
        }
        var ret = emitBlockStatement(obj)
        while (currentTransaction !== query)
            closeTransaction()
        console.debug("Query compiled:\n", ret)
        query.buildComplete()
    }

    function emitBlockStatement(obj) {
        var ret = []
        obj.body.forEach(function(el) { ret.push(invokeProc(el)) })
        return ret.join("")
    }

    function emitVariableDeclaration(obj) {
        var ret = []
        //        console.log("variable declaration", JSON.stringify(obj))
        obj.declarations.forEach(function(el) { ret.push(invokeProc(el)) })
        return ret
    }

    function emitPathReference(obj) {
        return "path(" + obj.pathIndex + ")"
    }

    function emitVariableDeclarator(obj) {
        return "var %1 = %2\n".arg(obj.id).arg(invokeProc(obj.init))
    }

    function emitPathInstance (obj) {
        console.log(obj.type, JSON.stringify(obj))
        addPathInstance(obj.pathIndex, obj.instance)
    }

    function emitReturnStatement(obj) {
        console.log("Building return statement", obj.argument)
        var ret = invokeProc(obj.argument)
        return "return run()"
        //        return scan(obj)
    }

    function emitBinaryExpression (obj, mode) {
//        console.log(obj.type, JSON.stringify(obj))
        var l = invokeProc(obj.left, 'expr')
        var op =obj.operator
        var r = invokeProc(obj.right, 'expr')
        if (!mode) {
            var expression =  l + " " + op + " " + r
            addExprFilter(expression)
        }
        return l + " " + op + " " + r
    }


    function emitPath(obj) {
        obj.steps.forEach(function(e) {invokeProc(e)})
        return currentTransaction;
    }

    function emitObjectType(obj, mode) {
        if (!mode)
            addTypeFilter(obj.value)
        else if (mode === 'expr')
            return "is(\"%1\")".arg(obj.value)

    }

    function emitExpressionStatement (obj) {
//        console.log(obj.type, JSON.stringify(obj, null, 2))
        var e = invokeProc(obj.expression)
        console.log("Expression statement", e)
        return "evaluate()"
    }


    function scan(obj) {
        var ret=[]
        console.log("Scanning object:", Object.keys(obj))
        for (var k in obj) {
            console.log("\tproperty", k+":", (typeof obj[k]), ((obj[k] instanceof Array) ? "[] =" : " ="), JSON.stringify(obj[k]))
            if (typeof obj[k] == "object")
                if (obj[k] instanceof Array) {
                    console.log("Found array", k, obj[k])
                    obj[k].forEach(function(el) { ret.push(scan(el))})
                } else if (obj[k] && obj[k].hasOwnProperty("type")) {
                    console.log("Found object", k, JSON.stringify(obj[k]))
                    ret.push(invokeProc(obj[k]))
                }
        }
        return ret
    }

    function emitRecursion (obj) {
        console.log("!!! IMPLEMENT RECURSION !!!")
    }


    function emitThisContext(obj) {
        return true
    }

    function emitContext (obj) {
        return invokeProc(obj.value)
    }

    function emitChildrenTraversal (obj) {
        return addTraverseChildNodes()
    }

    function emitTypeFilter(obj, seq) {
        return addTypeFilter(seq, obj.types[0].text)
    }

    function emitNodeStatement(obj) {
        invokeProc(obj.filter)
        if (obj.post)
            obj.post.forEach(function(e) { invokeProc(e) } )
    }

    function emitTraversal (obj) {
        //        scan(obj)
        switch(obj.direction) {
        case "NE":
            addTraverseNE(obj.kind)
            break;
        case "EN":
            addTraverseEN(obj.kind)
            break;
        default:
            console.log("unknown traversal", JSON.stringify(obj))
        }
    }

    function emitIndexReduce (obj) {
        return addIndexReduce(obj.value.value)
    }

    function emitEdgeStatement (obj) {
        invokeProc(obj.filter)
        if (obj.post)
            obj.post.forEach(function(e) { invokeProc(e) } )
    }

    function emitDoStatement (obj) {
        var ret = []
        obj.routines.forEach(function(e) { ret.push(invokeProc(e)) } )
        return ret
    }

    function emitScriptRoutine(obj) {
        //        console.log("script:\n" , JSON.stringify(obj.script, null, 2))
        var script = invokeProc(obj.script)
        console.log("Script expr:", script)
        //        return addScript(obj.script, true)
    }

    function emitCallExpression(obj) {
        var expr = invokeProc(obj.expression)
        var args = invokeProc(obj.arguments)
        var rhs = []
        obj.extra.forEach(function (el){
            var expr = invokeProc(el.expr)
            rhs.push(el.operator)
            //            console.log(JSON.stringify(el, null, 2))
            rhs.push(expr)
        })
        return expr + args + rhs.join("")
    }

    function emitIdentifier(obj) {
        return obj.name
    }

    function emitCaptureRoutine(obj) {
//        console.log(obj.type, JSON.stringify(obj, null, 2))
        return addCapture(invokeProc(obj.key), invokeProc(obj.expression), true)
    }

    function emitContextProperty (obj) {
        var ctx = invokeProc(obj.context)
        var prop = invokeProc(obj.property)
        console.log("Context property", ctx, prop)
    }

    function emitArguments(obj) {
        var ret = []
        if(obj.list) {
            obj.list.forEach(function(el) {
                ret.push(invokeProc(el))
            })
        }
        return "(" + ret.join(",") + ")"
    }

    function emitArrayLiteral(obj) {
        var ret = []
        if(obj.value) {
            obj.value.forEach(function(el) {
                ret.push(invokeProc(el))

            })
        }
        return ret.join(",")
    }

    function emitLiteral(obj) {
        return '"' + obj.value + '"'
    }

    function emitExecPath (obj) {
        addExecPath(obj.argument, obj.path.pathIndex)
        //        console.log("!!! EMIT EXEC PATH !!!", JSON.stringify(obj, null, 2))
    }


    function emitAdditiveExpression(obj) {
        var lhs = invokeProc(obj.lhs)
        var rhs = []
        obj.rhs.forEach(function (el){
            var expr = invokeProc(el.expr)
            rhs.push(el.operator)
            rhs.push(expr)
        })
        return lhs + rhs.join("")
    }

    function emitMultiplicativeExpression(obj) {
        return emitAdditiveExpression(obj)
    }

    function emitBracketedExpression(obj) {
        return "(" + invokeProc(obj.value) + ")"
    }

    function emitShiftExpression(obj) {
        return emitAdditiveExpression(obj)
    }

    function emitMemberExpression(obj) {
//        console.log("member expression", JSON.stringify(obj, null, 2))
        var o = invokeProc(obj.object)
        var prop = invokeProc(obj.property)
        return o + "." + prop
    }

    function emitSquareBracketedExpression(obj) {
        return "[" + invokeProc(obj.value) + "]"
    }

    function emitDotAccess(obj) {
        return "." + obj.value
    }

    function emitDollarIdentifier(obj) {
        // TODO: do something with that adorable identifier
        return obj.value
    }

    function emitAssignmentExpression(obj) {
        return invokeProc(obj.lhs) + obj.operator + invokeProc(obj.rhs)
    }

    function emitRoutineBlock(obj) {
        var last
        obj.statements.forEach(function(el) { last = invokeProc(el) })
        return last
    }

    function emitScopeSelector(obj) {
        if (!obj.scope)
            return null
        console.log("!!! EMIT SCOPE SELECTOR !!!", obj.scope)
    }

    property var invokeProc: function () {
        var obj = arguments[0]
        if (obj && obj.type) {
            var fnName = "emit" + obj.type
            if (typeof root[fnName] == "function") {
                //                console.debug("process", obj.type)
                return root[fnName](arguments[0], arguments[1])
            } else {
                console.error("\n\nfunction", fnName, "(obj) {\n\t\n}\n\nis not defined")
                return undefined
            }
        }
        return obj
    }


    function build(q, ast) {
        query = q
        invokeProc(ast)
    }

}

