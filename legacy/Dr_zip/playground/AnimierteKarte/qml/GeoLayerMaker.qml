import QtQuick 2.5
import Dr.MetaGraph 1.0
import Dr.Cartograph 1.0

ModelWeaver {
    id: i_layers
    delegate: GeoLayer {
        id: g_layer
        dataSource: i_ds.getDB(handle.outNodes("db")[0])
        table: handle.data.tableName
        parent: gview.item
        anchors.fill: parent
        visible: handle.data.visible
        property alias iGeomBuffer: i_geoms
        property NodeChildrenModel geomBuffers: childrenModel
        
        ModelWeaver {
            id: i_geoms
            active: i_layers.ready && g_layer.isValid
            model: g_layer.geomBuffers
            GeomBuffer {
                id: g_buf
                layer: g_layer
                property var node: handle
                property var attrs: handle.data.attrlist ? handle.data.attrlist : ""
                clause: handle.data.where
                Component.onCompleted: loadData(attrs)
                onReadyChanged: applyStyle()
                
                function applyStyle() {
                    if(ready) {
                        var styleList = node.outNodes("style")
                        if (styleList.length === 0)
                            return
                        var s = styleList[0]
                        style = i_styles.instance(s.modelIndex)
                    }
                }
                
                function loadData(attrs) {
                    if (attrs.length > 0)
                        var attrlist = attrs.split(" ")
                    else
                        attrlist = []
                    var cache = node.outNodes("cache")
                    if (cache.length === 0) {
                        fetchGeometry(attrlist)
                    } else {
                        console.log("loading cache for", clause, cache[0])
                        loadCache(cache[0].data)
                    }
                }
            }
        }
    }
}
