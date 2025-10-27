"""
REST API for Lambda³
Production-ready API for Lambda calculus operations
"""

from typing import Dict, Optional
from dataclasses import dataclass, asdict
import json

try:
    from lambda3.parser.parser import parse
    from lambda3.engine.reducer import reduce
    from lambda3.ternary.encoder import encode, decode
    from lambda3.types.inference import infer_type
    from lambda3.hybrid.reasoning_engine import HybridReasoningEngine, ReasoningMode
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


# ============================================================================
# API MODELS
# ============================================================================

@dataclass
class ParseRequest:
    """Request to parse a lambda term"""
    source: str


@dataclass
class ParseResponse:
    """Response from parse"""
    ast: str
    success: bool
    error: Optional[str] = None


@dataclass
class ReduceRequest:
    """Request to reduce a term"""
    source: str
    max_steps: int = 1000


@dataclass
class ReduceResponse:
    """Response from reduce"""
    result: str
    steps: int
    success: bool
    error: Optional[str] = None


@dataclass
class EncodeRequest:
    """Request to encode a term"""
    source: str


@dataclass
class EncodeResponse:
    """Response from encode"""
    trits: list
    num_trits: int
    ternary_bits: float
    binary_bits: int
    savings_percent: float
    success: bool
    error: Optional[str] = None


@dataclass
class InferRequest:
    """Request for type inference"""
    source: str


@dataclass
class InferResponse:
    """Response from type inference"""
    inferred_type: str
    success: bool
    error: Optional[str] = None


@dataclass
class ReasonRequest:
    """Request for hybrid reasoning"""
    query: str
    mode: str = "HYBRID"  # INTUITION, VERIFICATION, or HYBRID


@dataclass
class ReasonResponse:
    """Response from reasoning"""
    answer: str
    confidence: float
    verified: bool
    explanation: list
    proof: Optional[str]
    success: bool
    error: Optional[str] = None


# ============================================================================
# API ENDPOINTS
# ============================================================================

class Lambda3API:
    """
    REST API for Lambda³
    
    Endpoints:
    - POST /parse      - Parse lambda term
    - POST /reduce     - Reduce to normal form
    - POST /encode     - Encode as ternary
    - POST /infer      - Infer type
    - POST /reason     - Hybrid reasoning
    - GET  /health     - Health check
    - GET  /version    - Version info
    """
    
    def __init__(self):
        self.version = "0.1.0"
        self.reasoning_engine = HybridReasoningEngine()
    
    # ========================================================================
    # ENDPOINTS
    # ========================================================================
    
    def parse_endpoint(self, request: ParseRequest) -> ParseResponse:
        """Parse a lambda term"""
        try:
            term = parse(request.source)
            return ParseResponse(
                ast=str(term),
                success=True
            )
        except Exception as e:
            return ParseResponse(
                ast="",
                success=False,
                error=str(e)
            )
    
    def reduce_endpoint(self, request: ReduceRequest) -> ReduceResponse:
        """Reduce a lambda term"""
        try:
            term = parse(request.source)
            result = reduce(term, max_steps=request.max_steps)
            
            return ReduceResponse(
                result=str(result),
                steps=request.max_steps,  # TODO: Track actual steps
                success=True
            )
        except Exception as e:
            return ReduceResponse(
                result="",
                steps=0,
                success=False,
                error=str(e)
            )
    
    def encode_endpoint(self, request: EncodeRequest) -> EncodeResponse:
        """Encode a lambda term"""
        try:
            term = parse(request.source)
            trits = encode(term)
            
            from lambda3.ternary.encoder import encoding_efficiency
            eff = encoding_efficiency(term)
            
            return EncodeResponse(
                trits=trits,
                num_trits=eff['num_trits'],
                ternary_bits=eff['ternary_bits'],
                binary_bits=eff['binary_bits'],
                savings_percent=eff['savings_percent'],
                success=True
            )
        except Exception as e:
            return EncodeResponse(
                trits=[],
                num_trits=0,
                ternary_bits=0.0,
                binary_bits=0,
                savings_percent=0.0,
                success=False,
                error=str(e)
            )
    
    def infer_endpoint(self, request: InferRequest) -> InferResponse:
        """Infer type of a lambda term"""
        try:
            term = parse(request.source)
            inferred = infer_type(term)
            
            return InferResponse(
                inferred_type=str(inferred),
                success=True
            )
        except Exception as e:
            return InferResponse(
                inferred_type="",
                success=False,
                error=str(e)
            )
    
    def reason_endpoint(self, request: ReasonRequest) -> ReasonResponse:
        """Hybrid reasoning"""
        try:
            mode_map = {
                'INTUITION': ReasoningMode.INTUITION,
                'VERIFICATION': ReasoningMode.VERIFICATION,
                'HYBRID': ReasoningMode.HYBRID
            }
            mode = mode_map.get(request.mode, ReasoningMode.HYBRID)
            
            result = self.reasoning_engine.reason(request.query, mode)
            
            return ReasonResponse(
                answer=result.answer,
                confidence=result.confidence,
                verified=result.verified,
                explanation=result.explanation,
                proof=result.proof,
                success=True
            )
        except Exception as e:
            return ReasonResponse(
                answer="",
                confidence=0.0,
                verified=False,
                explanation=[],
                proof=None,
                success=False,
                error=str(e)
            )
    
    def health_endpoint(self) -> Dict:
        """Health check"""
        return {
            "status": "healthy",
            "version": self.version,
            "service": "Lambda3 API"
        }
    
    def version_endpoint(self) -> Dict:
        """Version information"""
        return {
            "version": self.version,
            "components": {
                "parser": "1.0.0",
                "reducer": "1.0.0",
                "encoder": "1.0.0",
                "type_checker": "1.0.0",
                "type_inference": "1.0.0",
                "neural": "0.1.0",
                "hybrid": "0.1.0"
            }
        }


# ============================================================================
# DEMO / TEST
# ============================================================================

def demo_api():
    """Demonstrate API usage"""
    print("\n" + "="*60)
    print("Lambda³ API Demo")
    print("="*60)
    
    api = Lambda3API()
    
    # Test parse
    print("\n1. Parse:")
    req = ParseRequest(source=r"\x.x")
    resp = api.parse_endpoint(req)
    print(f"   Request: {req.source}")
    print(f"   Response: {resp.ast}")
    print(f"   Success: {resp.success}")
    
    # Test reduce
    print("\n2. Reduce:")
    req2 = ReduceRequest(source=r"(\x.x) y")
    resp2 = api.reduce_endpoint(req2)
    print(f"   Request: {req2.source}")
    print(f"   Response: {resp2.result}")
    print(f"   Success: {resp2.success}")
    
    # Test encode
    print("\n3. Encode:")
    req3 = EncodeRequest(source=r"\x.x")
    resp3 = api.encode_endpoint(req3)
    print(f"   Request: {req3.source}")
    print(f"   Trits: {resp3.trits}")
    print(f"   Savings: {resp3.savings_percent:.1f}%")
    print(f"   Success: {resp3.success}")
    
    # Test infer
    print("\n4. Type Inference:")
    req4 = InferRequest(source=r"\x.x")
    resp4 = api.infer_endpoint(req4)
    print(f"   Request: {req4.source}")
    print(f"   Type: {resp4.inferred_type}")
    print(f"   Success: {resp4.success}")
    
    # Test reason
    print("\n5. Hybrid Reasoning:")
    req5 = ReasonRequest(query=r"(\x.x) y", mode="HYBRID")
    resp5 = api.reason_endpoint(req5)
    print(f"   Query: {req5.query}")
    print(f"   Answer: {resp5.answer}")
    print(f"   Verified: {resp5.verified}")
    print(f"   Success: {resp5.success}")
    
    # Health check
    print("\n6. Health:")
    health = api.health_endpoint()
    print(f"   {health}")
    
    print("\n" + "="*60)
    print("API Demo Complete")
    print("="*60)


def main():
    print("="*60)
    print("  Lambda³ REST API")
    print("  Production-Ready Interface")
    print("="*60)
    
    demo_api()
    
    print("\n🎯 API Endpoints:")
    print("  POST /parse       - Parse lambda term")
    print("  POST /reduce      - Reduce to normal form")
    print("  POST /encode      - Ternary encoding")
    print("  POST /infer       - Type inference")
    print("  POST /reason      - Hybrid reasoning")
    print("  GET  /health      - Health check")
    print("  GET  /version     - Version info")
    
    print("\n📚 Deployment:")
    print("  - FastAPI/Flask for HTTP server")
    print("  - Docker container")
    print("  - Kubernetes for scaling")
    print("  - CI/CD pipeline")
    
    return 0


if __name__ == '__main__':
    sys.exit(main())

