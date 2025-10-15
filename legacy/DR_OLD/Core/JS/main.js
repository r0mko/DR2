core.js.importExtension("qt.gui")
core.js.importExtension("qt.core")
core.js.importExtension("qt")
core.js.include("parsewiki.js")
function arrayMax()
{
    var max=0;
    this.forEach(function(item){
        if(item>max)
            max=item;
    })
    return max;
}
debugger

