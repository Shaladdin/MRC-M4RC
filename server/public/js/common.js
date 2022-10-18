function removeAllEvents(node) {
    let newNode = node.cloneNode(true);
    node.parentNode.replaceChild(newNode, node);
    return newNode;
}

// turn string into json SAFELY 
function objify(str) {
    try {
        return jsonify(String(str));
    } catch (e) {
        return false;
    }
}
