const { stringify, parse: jsonify } = JSON;

// turn string into json SAFELY 
function objify(str) {
    try {
        return jsonify(String(str));
    } catch (e) {
        console.warn(e, str);
        return false;
    }
}
async function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

// function updateButton() {
//     doc.roomDetection.button.style.backgroundColor = securityMode ? 'var(--lightGreen)' : 'var(--backGreen)'
//     doc.roomDetection.button.lastElementChild.textContent = securityMode ? 'Aktif' : 'mati:';
//     doc.roomDetection.button.lastElementChild.style.color = securityMode ? 'white' : 'var(--lighterBack)';
// }


class button {
    constructor(element, text, color, backColor, startingState, callBack) {
        this.button = element;
        this.text = text;
        this.color = color;
        this.backColor = backColor;
        this.callBack = callBack;
        this.state = startingState;

        this.update = (call = false, state_ = this.state) => {
            const { state, button: element, text, color, backColor, callBack, flick } = this;
            this.state = state_;
            var i = state ? 1 : 0;
            element.style.backgroundColor = backColor[i];
            element.lastElementChild.textContent = text[i];
            element.lastElementChild.style.color = color[i];
            if (call)
                callBack(this);
        }
        this.flick = () => {
            console.log('flick!')
            this.state = !this.state;
            this.update(true);
        }
        this.button.addEventListener('click', this.flick);
    }

}