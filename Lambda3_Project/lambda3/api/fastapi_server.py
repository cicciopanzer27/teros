"""
FastAPI HTTP Server for Lambda³ REST API
Production-ready web server
"""

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import Optional, List
import uvicorn

from lambda3.api.rest_api import (
    Lambda3API,
    ParseRequest, ParseResponse,
    ReduceRequest, ReduceResponse,
    EncodeRequest, EncodeResponse,
    InferRequest, InferResponse,
    ReasonRequest, ReasonResponse
)


# Initialize FastAPI app
app = FastAPI(
    title="Lambda³ API",
    description="REST API for Lambda Calculus operations with Ternary encoding",
    version="1.0.0"
)

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Configure appropriately for production
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Initialize Lambda³ API
lambda3_api = Lambda3API()


# ============================================================================
# ENDPOINTS
# ============================================================================

@app.get("/")
async def root():
    """Root endpoint"""
    return {
        "service": "Lambda³ API",
        "version": "1.0.0",
        "status": "operational",
        "endpoints": {
            "parse": "/parse",
            "reduce": "/reduce",
            "encode": "/encode",
            "infer": "/infer",
            "reason": "/reason",
            "health": "/health",
            "version": "/version"
        }
    }


@app.post("/parse", response_model=ParseResponse)
async def parse_endpoint(request: ParseRequest):
    """Parse a lambda term"""
    try:
        return lambda3_api.parse_endpoint(request)
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))


@app.post("/reduce", response_model=ReduceResponse)
async def reduce_endpoint(request: ReduceRequest):
    """Reduce a lambda term to normal form"""
    try:
        return lambda3_api.reduce_endpoint(request)
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))


@app.post("/encode", response_model=EncodeResponse)
async def encode_endpoint(request: EncodeRequest):
    """Encode a lambda term as ternary"""
    try:
        return lambda3_api.encode_endpoint(request)
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))


@app.post("/infer", response_model=InferResponse)
async def infer_endpoint(request: InferRequest):
    """Infer the type of a lambda term"""
    try:
        return lambda3_api.infer_endpoint(request)
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))


@app.post("/reason", response_model=ReasonResponse)
async def reason_endpoint(request: ReasonRequest):
    """Perform hybrid reasoning"""
    try:
        return lambda3_api.reason_endpoint(request)
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))


@app.get("/health")
async def health():
    """Health check endpoint"""
    return lambda3_api.health_endpoint()


@app.get("/version")
async def version():
    """Version information"""
    return lambda3_api.version_endpoint()


@app.get("/stats")
async def stats():
    """API statistics"""
    return {
        "total_requests": 0,  # Would track with middleware
        "cache_hits": 0,
        "average_response_time": 0,
    }


# ============================================================================
# MAIN
# ============================================================================

def main():
    """Run the FastAPI server"""
    print("="*60)
    print("  Lambda³ FastAPI Server")
    print("  Starting on http://0.0.0.0:8000")
    print("="*60)
    print("\nEndpoints:")
    print("  POST /parse      - Parse lambda term")
    print("  POST /reduce     - Reduce to normal form")
    print("  POST /encode     - Ternary encoding")
    print("  POST /infer      - Type inference")
    print("  POST /reason     - Hybrid reasoning")
    print("  GET  /health     - Health check")
    print("  GET  /version    - Version info")
    print("  GET  /stats      - Statistics")
    print("\nDocs:")
    print("  http://localhost:8000/docs")
    print("  http://localhost:8000/redoc")
    print("="*60)
    
    uvicorn.run(
        "lambda3.api.fastapi_server:app",
        host="0.0.0.0",
        port=8000,
        reload=True,
        log_level="info"
    )


if __name__ == "__main__":
    main()

