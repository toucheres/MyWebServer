let currentUser = null;
let socket = null;

// 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', function () {
    // 绑定表单提交事件
    document.getElementById('login').addEventListener('submit', handleLogin);
    document.getElementById('register').addEventListener('submit', handleRegister);
    document.getElementById('deregister').addEventListener('submit', handleDeregister);

    // 绑定回车键发送消息
    document.getElementById('messageInput').addEventListener('keypress', function (e) {
        if (e.key === 'Enter') {
            sendMessage();
        }
    });

    // 检查是否已登录
    checkLoginStatus();
});

// 处理登录
async function handleLogin(e) {
    e.preventDefault();
    const username = document.getElementById('loginUsername').value;
    const password = document.getElementById('loginPassword').value;

    try {
        const response = await fetch(`/login/${username}/${password}`, {
            credentials: 'include',
            mode: 'cors'
        });
        if (response.ok) {
            currentUser = username;
            showChat();
            await loadInitialMessages();
            connectWebSocket();
        } else {
            showError('登录失败，请检查用户名和密码');
        }
    } catch (error) {
        showError('网络错误，请稍后重试');
    }
}

// 处理注册
async function handleRegister(e) {
    e.preventDefault();
    const username = document.getElementById('registerUsername').value;
    const password = document.getElementById('registerPassword').value;

    try {
        const response = await fetch('/register', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: `name = '${username}', password = '${password}'`
        });

        if (response.ok) {
            showSuccess('注册成功！请登录');
            showLogin();
        } else if (response.status === 401) {
            showError('用户名已存在');
        } else {
            showError('注册失败，请稍后重试');
        }
    } catch (error) {
        showError('网络错误，请稍后重试');
    }
}

// 处理注销账号
async function handleDeregister(e) {
    e.preventDefault();
    const username = document.getElementById('deregisterUsername').value;
    const password = document.getElementById('deregisterPassword').value;

    try {
        const response = await fetch('/deregistration', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/x-www-form-urlencoded',
            },
            body: `name = '${username}', password = '${password}'`
        });

        if (response.ok) {
            showSuccess('账号注销成功');
            logout();
        } else {
            showError('注销失败，请检查用户名和密码');
        }
    } catch (error) {
        showError('网络错误，请稍后重试');
    }
}

// 加载初始消息（仅在登录时调用一次）
async function loadInitialMessages() {
    try {
        const response = await fetch('/getmessages', {
            credentials: 'include',
            mode: 'cors'
        });
        if (response.ok) {
            const text = await response.text();
            // console.log('服务器返回的原始数据:', text);
            
            // 直接传递完整的JSON字符串，而不是分割它
            displayMessages(text);
        } else if (response.status === 403) {
            logout();
            showError('登录已过期，请重新登录');
        }
    } catch (error) {
        console.error('获取消息失败:', error);
    }
}

// 连接WebSocket
function connectWebSocket() {
    if (socket) {
        socket.close();
    }

    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    const wsUrl = `${protocol}//${window.location.host}/api/ws`;
    
    socket = new WebSocket(wsUrl);
    
    socket.onopen = function(event) {
        console.log('WebSocket连接已建立');
        document.getElementById('messageInput').disabled = false;
        document.querySelector('.chat-input button').disabled = false;
    };
    
    socket.onmessage = function(event) {
        try {
            const messageText = event.data;
            // 解析 "username = {}, content = {}" 格式
            const match = messageText.match(/username = (.+?), content = (.+)/);
            if (match) {
                const message = {
                    sender: match[1],
                    content: match[2]
                };
                addMessage(message);
            }
        } catch (error) {
            console.error('解析WebSocket消息失败:', error);
        }
    };
    
    socket.onclose = function(event) {
        console.log('WebSocket连接已关闭');
        document.getElementById('messageInput').disabled = true;
        document.querySelector('.chat-input button').disabled = true;
        
        // 如果用户仍在聊天界面，尝试重连
        if (currentUser && !document.getElementById('chatContainer').classList.contains('hidden')) {
            setTimeout(connectWebSocket, 3000);
        }
    };
    
    socket.onerror = function(error) {
        console.error('WebSocket错误:', error);
        showError('连接错误，请刷新页面重试');
    };
}

// 显示消息
// function displayMessages(messages) {
//     const messagesContainer = document.getElementById('messages');
//     messagesContainer.innerHTML = '';

//     messages.forEach(messageJson => {
//         try {
//             const message = JSON.parse(messageJson);
//             addMessage(message);
//         } catch (error) {
//             console.error('解析消息失败:', error);
//         }
//     });
// }
function displayMessages(messagesText) {
    const messagesContainer = document.getElementById('messages');
    messagesContainer.innerHTML = '';

    try {
        // 直接解析JSON数组
        const messages = JSON.parse(messagesText);

        // 遍历消息数组
        messages.forEach(message => {
            addMessage(message);
        });
    } catch (error) {
        console.error('解析消息失败:', error);
        console.log('原始数据:', messagesText);
    }
}

// 添加单条消息
function addMessage(message) {
    const messagesContainer = document.getElementById('messages');
    const messageDiv = document.createElement('div');
    messageDiv.className = 'message';
    messageDiv.innerHTML = `
        <div class="sender">${message.sender}</div>
        <div class="content">${message.content}</div>
    `;
    messagesContainer.appendChild(messageDiv);
    
    // 滚动到底部
    messagesContainer.scrollTop = messagesContainer.scrollHeight;
}

// 发送消息
function sendMessage() {
    const messageInput = document.getElementById('messageInput');
    const content = messageInput.value.trim();
    
    if (!content || !socket || socket.readyState !== WebSocket.OPEN) {
        return;
    }
    
    // 按照指定格式发送消息: "username = {}, content = {}"
    const message = `username = ${currentUser}, content = ${content}`;
    
    try {
        socket.send(message);
        messageInput.value = '';
    } catch (error) {
        console.error('发送消息失败:', error);
        showError('发送失败，请重试');
    }
}

// 显示登录界面
function showLogin() {
    hideAll();
    document.getElementById('loginForm').classList.remove('hidden');
}

// 显示注册界面
function showRegister() {
    hideAll();
    document.getElementById('registerForm').classList.remove('hidden');
}

// 显示聊天界面
function showChat() {
    hideAll();
    document.getElementById('chatContainer').classList.remove('hidden');
    document.getElementById('currentUser').textContent = currentUser;
    document.getElementById('messageInput').disabled = false;
    document.querySelector('.chat-input button').disabled = false;
}

// 显示注销界面
function showDeregister() {
    hideAll();
    document.getElementById('deregisterForm').classList.remove('hidden');
}

// 返回聊天界面
function backToChat() {
    showChat();
}

// 隐藏所有界面
function hideAll() {
    const containers = ['loginForm', 'registerForm', 'chatContainer', 'deregisterForm'];
    containers.forEach(id => {
        document.getElementById(id).classList.add('hidden');
    });
}

// 退出登录
function logout() {
    currentUser = null;
    if (socket) {
        socket.close();
        socket = null;
    }
    showLogin();
}

// 检查登录状态
async function checkLoginStatus() {
    try {
        const response = await fetch('/do', {
            credentials: 'include',
            mode: 'cors'
        });
        if (response.ok) {
            const text = await response.text();
            const match = text.match(/your name is: ([^,]+)/);
            if (match) {
                currentUser = match[1];
                showChat();
                await loadInitialMessages();
                connectWebSocket();
                return;
            }
        }
    } catch (error) {
        console.error('检查登录状态失败:', error);
    }
    showLogin();
}

// 显示错误消息
function showError(message) {
    removeMessages();
    const errorDiv = document.createElement('div');
    errorDiv.className = 'error';
    errorDiv.textContent = message;

    const activeForm = document.querySelector('.form-container:not(.hidden)');
    if (activeForm) {
        activeForm.insertBefore(errorDiv, activeForm.firstChild);
    }
}

// 显示成功消息
function showSuccess(message) {
    removeMessages();
    const successDiv = document.createElement('div');
    successDiv.className = 'success';
    successDiv.textContent = message;

    const activeForm = document.querySelector('.form-container:not(.hidden)');
    if (activeForm) {
        activeForm.insertBefore(successDiv, activeForm.firstChild);
    }
}

// 移除消息提示
function removeMessages() {
    const messages = document.querySelectorAll('.error, .success');
    messages.forEach(msg => msg.remove());
}