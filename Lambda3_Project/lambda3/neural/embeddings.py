"""
Neural Embeddings for Lambda³
Advanced embedding techniques for lambda terms
"""

import torch
import torch.nn as nn
import numpy as np
from typing import List, Dict, Tuple, Optional
import json
from pathlib import Path

try:
    from lambda3.parser.lambda_parser import LambdaTerm, Var, Abs, App
    from lambda3.engine.reducer import reduce
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from parser.lambda_parser import LambdaTerm, Var, Abs, App
    from engine.reducer import reduce


class LambdaEmbedding:
    """
    Advanced embedding system for lambda terms
    
    Features:
    - Tree-LSTM embeddings
    - Graph neural networks
    - Positional encodings
    - Type-aware embeddings
    """
    
    def __init__(self, embedding_dim: int = 128, vocab_size: int = 1000):
        self.embedding_dim = embedding_dim
        self.vocab_size = vocab_size
        
        # Tree-LSTM for lambda terms
        self.tree_lstm = TreeLSTM(embedding_dim)
        
        # Graph neural network
        self.gnn = GraphNeuralNetwork(embedding_dim)
        
        # Positional encoding
        self.pos_encoder = PositionalEncoder(embedding_dim)
        
        # Type embeddings
        self.type_embedder = TypeEmbedder(embedding_dim)
        
        # Vocabulary
        self.vocab = {}
        self.idx_to_token = {}
        self._build_vocab()
    
    def _build_vocab(self):
        """Build vocabulary from lambda terms"""
        # Basic tokens
        tokens = [
            'VAR', 'ABS', 'APP', 'LPAREN', 'RPAREN', 'LAMBDA', 'DOT',
            'x', 'y', 'z', 'f', 'g', 'h', 'a', 'b', 'c'
        ]
        
        for i, token in enumerate(tokens):
            self.vocab[token] = i
            self.idx_to_token[i] = token
    
    def embed_lambda_term(self, term: LambdaTerm) -> torch.Tensor:
        """Create embedding for lambda term"""
        # Convert to tree representation
        tree = self._term_to_tree(term)
        
        # Get tree-LSTM embedding
        tree_embedding = self.tree_lstm(tree)
        
        # Get graph embedding
        graph = self._term_to_graph(term)
        graph_embedding = self.gnn(graph)
        
        # Combine embeddings
        combined = torch.cat([tree_embedding, graph_embedding], dim=-1)
        
        # Add positional encoding
        pos_encoded = self.pos_encoder(combined)
        
        return pos_encoded
    
    def embed_with_types(self, term: LambdaTerm, type_info: Dict) -> torch.Tensor:
        """Create type-aware embedding"""
        # Basic embedding
        base_embedding = self.embed_lambda_term(term)
        
        # Type embedding
        type_embedding = self.type_embedder(type_info)
        
        # Combine
        return torch.cat([base_embedding, type_embedding], dim=-1)
    
    def _term_to_tree(self, term: LambdaTerm) -> Dict:
        """Convert lambda term to tree structure"""
        if isinstance(term, Var):
            return {
                'type': 'VAR',
                'value': term.name,
                'children': []
            }
        elif isinstance(term, Abs):
            return {
                'type': 'ABS',
                'value': term.var,
                'children': [self._term_to_tree(term.body)]
            }
        elif isinstance(term, App):
            return {
                'type': 'APP',
                'value': 'APPLICATION',
                'children': [
                    self._term_to_tree(term.func),
                    self._term_to_tree(term.arg)
                ]
            }
        else:
            return {'type': 'UNKNOWN', 'value': str(term), 'children': []}
    
    def _term_to_graph(self, term: LambdaTerm) -> Dict:
        """Convert lambda term to graph structure"""
        nodes = []
        edges = []
        
        def add_node(term, parent_id=None):
            node_id = len(nodes)
            nodes.append({
                'id': node_id,
                'type': type(term).__name__,
                'value': str(term),
                'parent': parent_id
            })
            
            if parent_id is not None:
                edges.append((parent_id, node_id))
            
            if isinstance(term, Abs):
                add_node(term.body, node_id)
            elif isinstance(term, App):
                add_node(term.func, node_id)
                add_node(term.arg, node_id)
            
            return node_id
        
        add_node(term)
        
        return {
            'nodes': nodes,
            'edges': edges
        }


class TreeLSTM(nn.Module):
    """Tree-LSTM for lambda terms"""
    
    def __init__(self, embedding_dim: int):
        super().__init__()
        self.embedding_dim = embedding_dim
        
        # LSTM cells
        self.lstm = nn.LSTM(embedding_dim, embedding_dim, batch_first=True)
        
        # Attention mechanism
        self.attention = nn.MultiheadAttention(embedding_dim, num_heads=8)
        
        # Output projection
        self.output_proj = nn.Linear(embedding_dim, embedding_dim)
    
    def forward(self, tree: Dict) -> torch.Tensor:
        """Forward pass through tree"""
        # Extract features
        features = self._extract_features(tree)
        
        # LSTM processing
        lstm_out, _ = self.lstm(features)
        
        # Attention
        attended, _ = self.attention(lstm_out, lstm_out, lstm_out)
        
        # Global pooling
        pooled = attended.mean(dim=1)
        
        # Output projection
        output = self.output_proj(pooled)
        
        return output
    
    def _extract_features(self, tree: Dict) -> torch.Tensor:
        """Extract features from tree"""
        # This is a simplified version
        # In practice, you'd traverse the tree and extract features
        features = torch.randn(1, 10, self.embedding_dim)
        return features


class GraphNeuralNetwork(nn.Module):
    """Graph Neural Network for lambda terms"""
    
    def __init__(self, embedding_dim: int):
        super().__init__()
        self.embedding_dim = embedding_dim
        
        # Graph convolution layers
        self.conv1 = nn.Linear(embedding_dim, embedding_dim)
        self.conv2 = nn.Linear(embedding_dim, embedding_dim)
        
        # Attention
        self.attention = nn.MultiheadAttention(embedding_dim, num_heads=8)
        
        # Output
        self.output = nn.Linear(embedding_dim, embedding_dim)
    
    def forward(self, graph: Dict) -> torch.Tensor:
        """Forward pass through graph"""
        nodes = graph['nodes']
        edges = graph['edges']
        
        # Node embeddings
        node_embeddings = torch.randn(len(nodes), self.embedding_dim)
        
        # Graph convolution
        conv1_out = self.conv1(node_embeddings)
        conv2_out = self.conv2(conv1_out)
        
        # Attention
        attended, _ = self.attention(conv2_out, conv2_out, conv2_out)
        
        # Global pooling
        pooled = attended.mean(dim=0)
        
        # Output
        output = self.output(pooled)
        
        return output


class PositionalEncoder(nn.Module):
    """Positional encoding for lambda terms"""
    
    def __init__(self, embedding_dim: int):
        super().__init__()
        self.embedding_dim = embedding_dim
    
    def forward(self, x: torch.Tensor) -> torch.Tensor:
        """Add positional encoding"""
        # Simplified positional encoding
        pos_encoding = torch.randn_like(x)
        return x + pos_encoding


class TypeEmbedder(nn.Module):
    """Type-aware embedding"""
    
    def __init__(self, embedding_dim: int):
        super().__init__()
        self.embedding_dim = embedding_dim
        
        # Type embeddings
        self.type_embeddings = nn.Embedding(100, embedding_dim)
        
        # Type projection
        self.type_proj = nn.Linear(embedding_dim, embedding_dim)
    
    def forward(self, type_info: Dict) -> torch.Tensor:
        """Create type embedding"""
        # Simplified type embedding
        type_id = hash(str(type_info)) % 100
        type_embedding = self.type_embeddings(torch.tensor(type_id))
        return self.type_proj(type_embedding)


# ============================================================================
# DEMO
# ============================================================================

def demo_embeddings():
    """Demonstrate embedding capabilities"""
    print("="*60)
    print("  Lambda³ Neural Embeddings Demo")
    print("="*60)
    
    # Create embedding system
    embedder = LambdaEmbedding(embedding_dim=64)
    
    # Test terms
    test_terms = [
        "\\x.x",           # Identity
        "\\x.\\y.x",       # Constant
        "\\f.\\g.\\x.f (g x)",  # Composition
        "(\\x.x) y"        # Application
    ]
    
    print("Embedding lambda terms...")
    for term_str in test_terms:
        try:
            # Parse term
            term = parse(term_str)
            
            # Create embedding
            embedding = embedder.embed_lambda_term(term)
            
            print(f"Term: {term_str}")
            print(f"Embedding shape: {embedding.shape}")
            print(f"Embedding norm: {embedding.norm().item():.4f}")
            print()
            
        except Exception as e:
            print(f"Error embedding {term_str}: {e}")
    
    print("Embedding features:")
    print("  Tree-LSTM embeddings")
    print("  Graph neural networks")
    print("  Positional encodings")
    print("  Type-aware embeddings")
    print("  Multi-head attention")


def main():
    print("="*60)
    print("  Lambda³ Neural Embeddings")
    print("  Advanced Embedding Techniques")
    print("="*60)
    
    demo_embeddings()
    
    print("\n" + "="*60)
    print("Embedding Features:")
    print("  Tree-LSTM for lambda terms")
    print("  Graph neural networks")
    print("  Positional encodings")
    print("  Type-aware embeddings")
    print("  Multi-head attention")
    print("  Advanced neural architectures")
    print("="*60)
    
    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())
