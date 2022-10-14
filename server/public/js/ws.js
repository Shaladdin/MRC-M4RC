const init = () => new Promise((res, rej) => {
    const isSecure = window.location.protocol == 'https:';
    const server = window.location.host;
    const socket = new WebSocket(`ws${isSecure ? 's' : ''}://${server}`);
    socket.addEventListener('open', (event) => {
        console.log('connected to webSocket')
        res(socket);
    })
    socket.addEventListener('message', (event) => {
        console.log(`server: ${event.data}`);
    })
})