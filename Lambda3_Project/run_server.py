"""
Run Lambda³ FastAPI Server
Production HTTP server
"""

import sys
import os
import uvicorn
from pathlib import Path

# Add Lambda³ to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def main():
    print("="*60)
    print("  Lambda³ FastAPI Server")
    print("  Starting HTTP Server...")
    print("="*60)
    
    print("\n🚀 Server Configuration:")
    print("  Host: 0.0.0.0")
    print("  Port: 8000")
    print("  Mode: Development")
    print("  Reload: True")
    
    print("\n📡 Available Endpoints:")
    print("  GET  /           - Root info")
    print("  POST /parse      - Parse lambda term")
    print("  POST /reduce     - Reduce to normal form")
    print("  POST /encode     - Ternary encoding")
    print("  POST /infer      - Type inference")
    print("  POST /reason     - Hybrid reasoning")
    print("  GET  /health     - Health check")
    print("  GET  /version    - Version info")
    print("  GET  /stats      - Statistics")
    
    print("\n📚 Documentation:")
    print("  http://localhost:8000/docs")
    print("  http://localhost:8000/redoc")
    
    print("\n🔧 Testing:")
    print("  curl http://localhost:8000/health")
    print("  curl -X POST http://localhost:8000/parse -H 'Content-Type: application/json' -d '{\"term\": \"\\\\x.x\"}'")
    
    print("\n" + "="*60)
    print("  Starting server...")
    print("="*60)
    
    try:
        uvicorn.run(
            "lambda3.api.fastapi_server:app",
            host="0.0.0.0",
            port=8000,
            reload=True,
            log_level="info",
            access_log=True
        )
    except KeyboardInterrupt:
        print("\n🛑 Server stopped by user")
    except Exception as e:
        print(f"\n❌ Server error: {e}")
        return 1
    
    return 0


if __name__ == '__main__':
    sys.exit(main())
