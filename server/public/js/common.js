function removeAllEvents(node){
    let newNode = node.cloneNode(true);
    node.parentNode.replaceChild(newNode, node);
    return newNode;
}