# Architecture 

## Endpoint Architecture 

```mermaid
erDiagram
    UiProc }|--|{ SecureProc : "unencrypted"
    SecureProc
    Storage }|--|{ SecureProc : "data"
    TrashCompactor }o--|{ SecureProc : "data"
    NetProc }|--|{ SecureProc : "encrypted"
```
    
# Network Archtecture

