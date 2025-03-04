# NotificationManager

NotificationManager is a Teamspeak client plugin that lets you toggle messages and pokes globally on the fly. It includes private, channel, server chats and pokes.

## Features
- Toggle pokes including an automated notification to the sender
- Toggle private chats including an automated notification to the sender
- Toggle channel chats
- Toggle server chats

## Installation & Execution
### Requirements
- Teamspeak 3 Client 3.6.2
- CMake 3.30

### Build process
```bash
  cmake .
  
  cmake --build .
  ```

### Execution
1. Move 'NotificationManager.dll' into your Teamspeak plugins directory
2. Create a folder named 'NotificationManager' in the plugins directory
3. Move all images from 'src/icons/' into the created 'NotificationManager' directory
4. Start your Teamspeak client

## Controls
| Button               | Description                      |
|----------------------|----------------------------------|
| Toggle Private Chats | Toggle incoming private messages |
| Toggle Channel Chats | Toggle incoming channel messages |
| Toggle Server Chats  | Toggle incoming server messages  |
| Toggle Pokes         | Toggle incoming pokes            |

- Buttons can be accessed by clicking on the 'Plugins' button on the top bar

## Contributors
- **EricZones** - Developer