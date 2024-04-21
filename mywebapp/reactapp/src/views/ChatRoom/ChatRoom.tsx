import React, { useEffect, useState } from 'react';

interface Message {
  sender: 'me' | 'them';
  content: string;
}

const ChatRoom: React.FC = () => {
  const [peerConnection, setPeerConnection] = useState<RTCPeerConnection | null>(null);
  const [dataChannel, setDataChannel] = useState<RTCDataChannel | null>(null);
  const [messages, setMessages] = useState<Message[]>([]);
  const [text, setText] = useState('');
  const [ws, setWs] = useState<WebSocket | null>(null);

  useEffect(() => {
    // Assume createPeerConnection() and setupWebSocket() are implemented
    const pc = createPeerConnection();
    setupWebSocket(pc);
    setPeerConnection(pc);

    return () => {
      pc.close();
    };
  }, []);

  const createPeerConnection = (): RTCPeerConnection => {
    const pc = new RTCPeerConnection({
        iceServers: [
          { urls: "stun:stun.example.com" },
          {
            urls: "turn:turn.example.com",
            username: "turnUsername",
            credential: "turnCredentials"
          }
        ]
      });

    pc.onicecandidate = (event) => {
      if (event.candidate) {
        // Send ICE candidate to peer via signaling server
        sendSignalingMessage({ type: 'candidate', candidate: event.candidate });
      }
      else{
        console.log("ICE Gathering Finished");
      }
    };
    pc.createOffer().then(offer => {
        console.log('createOffer opened');
        pc.setLocalDescription(offer);
        // Send the offer to the remote peer
      });

    pc.ondatachannel = (event) => {
      const receiveChannel = event.channel;
      receiveChannel.onmessage = (e) => receiveMessage(e.data);
      receiveChannel.onopen = () => console.log('Data channel opened');
      receiveChannel.onclose = () => console.log('Data channel closed');
      setDataChannel(receiveChannel);
    };
    pc.oniceconnectionstatechange = () => {
        console.log(pc.iceConnectionState);
        if (pc.iceConnectionState === "connected" || 
        pc.iceConnectionState === "completed") {
          // Now the data channel should be able to transition to "open"
        }
      };
      
    return pc;
  };

  const setupWebSocket = (pc: RTCPeerConnection) => {
    const ws = new WebSocket('ws://192.168.3.61:3001');
    setWs(ws);
    ws.onopen = () => {
      // You might want to send a "join" message to the signaling server here
      console.log('ws opened');
    //   ws.send('Hello Server!');
    };
    ws.onerror = (error) => {
      console.error('WebSocket error:', error);
    };
    

    ws.onmessage = async (event) => {
      const message = JSON.parse(event.data);
      switch (message.type) {
        case 'offer':
          // await pc.setRemoteDescription(new RTCSessionDescription(message.offer));
          // const answer = await pc.createAnswer();
          // await pc.setLocalDescription(answer);
          // ws.send(JSON.stringify({ type: 'answer', answer }));
          // setMessages([...messages, { sender: 'them', content: message.text }]);
          setMessages(prev => [...prev, { sender: 'them', content: message.text }]);
          break;
        case 'answer':
          await pc.setRemoteDescription(new RTCSessionDescription(message.answer));
          break;
        case 'candidate':
          await pc.addIceCandidate(new RTCIceCandidate(message.candidate));
          break;
        default:
          break;
      }
    };
  };

  const sendSignalingMessage = (message: Object) => {
    // Implement based on your signaling mechanism
    // For WebSocket: ws.send(JSON.stringify(message));
  };

  const createDataChannel = () => {
    if (peerConnection) {
      const channel = peerConnection.createDataChannel('chat');
      channel.onopen = () => console.log('Data channel opened');
      channel.onmessage = (e) => receiveMessage(e.data);
      console.log('createDataChannel created');
      setDataChannel(channel);
    }
  };

  const sendMessage = () => {
    // if (dataChannel && dataChannel.readyState === 'open' && text.trim() !== '') {
    //   dataChannel.send(text);
    //   setMessages([...messages, { sender: 'me', content: text }]);
    //   setText('');
    // }
    // else{
    //     console.log(dataChannel?.readyState);
    // }
    if (ws) {
      ws.send(JSON.stringify({ type: 'offer', text }));
      setMessages([...messages, { sender: 'me', content: text }]);
      setText('');// Clear message input after sending
    }

  };

  const receiveMessage = (text: string) => {
    setMessages([...messages, { sender: 'them', content: text }]);
  };

  return (
    <div>
      <div>
        {messages.map((msg, index) => (
          <p key={index}>
            <b>{msg.sender === 'me' ? 'Me: ' : 'Them: '}</b>
            {msg.content}
          </p>
        ))}
      </div>
      <input type="text" value={text} onChange={(e) => setText(e.target.value)} />
      <button onClick={sendMessage}>Send</button>
      <button onClick={createDataChannel}>Create Data Channel</button>
    </div>
  );
};

export default ChatRoom;
