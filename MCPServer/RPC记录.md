以`mcp_server_time`为例

### client请求
`{"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{"tools":true,"prompts":false,"resources":true,"logging":false,"roots":{"listChanged":false}},"clientInfo":{"name":"cursor-vscode","version":"1.0.0"}},"jsonrpc":"2.0","id":0}`

### server响应
`{"jsonrpc":"2.0","id":0,"result":{"protocolVersion":"2024-11-05","capabilities":{"experimental":{},"tools":{"listChanged":false}},"serverInfo":{"name":"mcp-time","version":"1.5.0"}}}`

### client通知
`{"method":"notifications/initialized","jsonrpc":"2.0"}`

### client请求
`{"method":"tools/list","jsonrpc":"2.0","id":1}`

### server响应
`{"jsonrpc":"2.0","id":1,"result":{"tools":[{"name":"get_current_time","description":"Get current time in a specific timezones","inputSchema":{"type":"object","properties":{"timezone":{"type":"string","description":"IANA timezone name (e.g., 'America/New_York', 'Europe/London'). Use 'America/New_York' as local timezone if no timezone provided by the user."}},"required":["timezone"]}}]}}`

### client请求
`{"method":"tools/call","params":{"name":"getCurrentTime","arguments":{"timezone":"Asia/Shanghai"}},"jsonrpc":"2.0","id":2}`

### server响应
`{"jsonrpc":"2.0","id":2,"result":{"content":[{"type":"text","text":"{\n  \"timezone\": \"Asia/Shanghai\",\n  \"datetime\": \"2025-03-24T15:21:18+08:00\",\n  \"is_dst\": false\n}"}],"isError":false}}`