"""
Neural Model for Tactic Suggestion
PyTorch implementation for training
"""

import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import Dataset, DataLoader
import json
from typing import List, Dict, Tuple
import numpy as np
from pathlib import Path

try:
    from lambda3.neural.dataset import ProofExample, ProofStep
except ImportError:
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from neural.dataset import ProofExample, ProofStep


# ============================================================================
# NEURAL ARCHITECTURE
# ============================================================================

class TacticSuggestionModel(nn.Module):
    """
    Neural network for suggesting proof tactics
    
    Architecture:
    - Input: Goal type + Context
    - Embedding: Goal type → vector
    - LSTM: Context sequence → hidden state
    - Attention: Focus on relevant context
    - Output: Tactic probabilities
    """
    
    def __init__(self, vocab_size: int, embedding_dim: int = 128, 
                 hidden_dim: int = 256, num_tactics: int = 8):
        super().__init__()
        
        self.embedding_dim = embedding_dim
        self.hidden_dim = hidden_dim
        self.num_tactics = num_tactics
        
        # Goal type embedding
        self.goal_embedding = nn.Embedding(vocab_size, embedding_dim)
        
        # Context LSTM
        self.context_lstm = nn.LSTM(embedding_dim, hidden_dim, 
                                   batch_first=True, bidirectional=True)
        
        # Attention mechanism
        self.attention = nn.MultiheadAttention(hidden_dim * 2, num_heads=8)
        
        # Output layers
        self.classifier = nn.Sequential(
            nn.Linear(hidden_dim * 2, hidden_dim),
            nn.ReLU(),
            nn.Dropout(0.3),
            nn.Linear(hidden_dim, hidden_dim // 2),
            nn.ReLU(),
            nn.Dropout(0.3),
            nn.Linear(hidden_dim // 2, num_tactics)
        )
        
        # Initialize weights
        self._init_weights()
    
    def _init_weights(self):
        """Initialize model weights"""
        for module in self.modules():
            if isinstance(module, nn.Linear):
                nn.init.xavier_uniform_(module.weight)
                if module.bias is not None:
                    nn.init.zeros_(module.bias)
    
    def forward(self, goal_types: torch.Tensor, contexts: torch.Tensor) -> torch.Tensor:
        """
        Forward pass
        
        Args:
            goal_types: [batch_size] - goal type indices
            contexts: [batch_size, seq_len] - context sequences
            
        Returns:
            [batch_size, num_tactics] - tactic probabilities
        """
        batch_size = goal_types.size(0)
        
        # Embed goal types
        goal_emb = self.goal_embedding(goal_types)  # [batch_size, embedding_dim]
        
        # Process context with LSTM
        context_emb = self.goal_embedding(contexts)  # [batch_size, seq_len, embedding_dim]
        lstm_out, _ = self.context_lstm(context_emb)  # [batch_size, seq_len, hidden_dim*2]
        
        # Apply attention
        goal_emb_expanded = goal_emb.unsqueeze(1)  # [batch_size, 1, embedding_dim]
        goal_emb_expanded = goal_emb_expanded.expand(-1, lstm_out.size(1), -1)
        
        # Self-attention on context
        attended, _ = self.attention(lstm_out, lstm_out, lstm_out)
        
        # Global average pooling
        pooled = attended.mean(dim=1)  # [batch_size, hidden_dim*2]
        
        # Combine goal and context
        combined = torch.cat([goal_emb, pooled], dim=1)  # [batch_size, embedding_dim + hidden_dim*2]
        
        # Project to hidden_dim*2
        combined = nn.Linear(combined.size(1), self.hidden_dim * 2).to(combined.device)(combined)
        
        # Classify tactics
        logits = self.classifier(combined)
        
        return logits


# ============================================================================
# DATASET CLASS
# ============================================================================

class TacticDataset(Dataset):
    """Dataset for tactic suggestion training"""
    
    def __init__(self, data_file: str, vocab: Dict[str, int], tactics: List[str]):
        self.vocab = vocab
        self.tactics = tactics
        self.tactic_to_idx = {t: i for i, t in enumerate(tactics)}
        
        # Load data
        with open(data_file, 'r') as f:
            data = json.load(f)
        
        self.examples = []
        for proof in data['proofs']:
            for step in proof['steps']:
                self.examples.append({
                    'goal_type': step['goal_type'],
                    'context': step['context'],
                    'tactic': step['tactic_used']
                })
    
    def __len__(self):
        return len(self.examples)
    
    def __getitem__(self, idx):
        example = self.examples[idx]
        
        # Convert to indices
        goal_type_idx = self.vocab.get(example['goal_type'], 0)
        context_indices = [self.vocab.get(var, 0) for var, _ in example['context']]
        
        # Pad context to fixed length
        max_context_len = 10
        if len(context_indices) > max_context_len:
            context_indices = context_indices[:max_context_len]
        else:
            context_indices.extend([0] * (max_context_len - len(context_indices)))
        
        tactic_idx = self.tactic_to_idx.get(example['tactic'], 0)
        
        return {
            'goal_type': torch.tensor(goal_type_idx, dtype=torch.long),
            'context': torch.tensor(context_indices, dtype=torch.long),
            'tactic': torch.tensor(tactic_idx, dtype=torch.long)
        }


# ============================================================================
# TRAINING
# ============================================================================

class TacticTrainer:
    """Trainer for tactic suggestion model"""
    
    def __init__(self, model: TacticSuggestionModel, device: str = 'cpu'):
        self.model = model.to(device)
        self.device = device
        self.optimizer = optim.Adam(model.parameters(), lr=0.001)
        self.criterion = nn.CrossEntropyLoss()
        self.scheduler = optim.lr_scheduler.ReduceLROnPlateau(
            self.optimizer, mode='min', patience=5, factor=0.5
        )
    
    def train_epoch(self, dataloader: DataLoader) -> float:
        """Train for one epoch"""
        self.model.train()
        total_loss = 0.0
        correct = 0
        total = 0
        
        for batch in dataloader:
            goal_types = batch['goal_type'].to(self.device)
            contexts = batch['context'].to(self.device)
            tactics = batch['tactic'].to(self.device)
            
            self.optimizer.zero_grad()
            
            # Forward pass
            logits = self.model(goal_types, contexts)
            loss = self.criterion(logits, tactics)
            
            # Backward pass
            loss.backward()
            torch.nn.utils.clip_grad_norm_(self.model.parameters(), max_norm=1.0)
            self.optimizer.step()
            
            total_loss += loss.item()
            _, predicted = torch.max(logits, 1)
            total += tactics.size(0)
            correct += (predicted == tactics).sum().item()
        
        accuracy = correct / total
        avg_loss = total_loss / len(dataloader)
        
        return avg_loss, accuracy
    
    def evaluate(self, dataloader: DataLoader) -> Tuple[float, float]:
        """Evaluate model"""
        self.model.eval()
        total_loss = 0.0
        correct = 0
        total = 0
        
        with torch.no_grad():
            for batch in dataloader:
                goal_types = batch['goal_type'].to(self.device)
                contexts = batch['context'].to(self.device)
                tactics = batch['tactic'].to(self.device)
                
                logits = self.model(goal_types, contexts)
                loss = self.criterion(logits, tactics)
                
                total_loss += loss.item()
                _, predicted = torch.max(logits, 1)
                total += tactics.size(0)
                correct += (predicted == tactics).sum().item()
        
        accuracy = correct / total
        avg_loss = total_loss / len(dataloader)
        
        return avg_loss, accuracy
    
    def train(self, train_loader: DataLoader, val_loader: DataLoader, 
              epochs: int = 50) -> Dict:
        """Full training loop"""
        history = {
            'train_loss': [], 'train_acc': [],
            'val_loss': [], 'val_acc': []
        }
        
        best_val_acc = 0.0
        
        for epoch in range(epochs):
            # Train
            train_loss, train_acc = self.train_epoch(train_loader)
            
            # Validate
            val_loss, val_acc = self.evaluate(val_loader)
            
            # Update scheduler
            self.scheduler.step(val_loss)
            
            # Save best model
            if val_acc > best_val_acc:
                best_val_acc = val_acc
                torch.save(self.model.state_dict(), 'best_tactic_model.pth')
            
            # Record history
            history['train_loss'].append(train_loss)
            history['train_acc'].append(train_acc)
            history['val_loss'].append(val_loss)
            history['val_acc'].append(val_acc)
            
            if epoch % 10 == 0:
                print(f"Epoch {epoch:3d}: "
                      f"Train Loss: {train_loss:.4f}, Train Acc: {train_acc:.4f}, "
                      f"Val Loss: {val_loss:.4f}, Val Acc: {val_acc:.4f}")
        
        return history


# ============================================================================
# INFERENCE
# ============================================================================

class TacticPredictor:
    """Inference for trained model"""
    
    def __init__(self, model_path: str, vocab: Dict[str, int], tactics: List[str]):
        self.vocab = vocab
        self.tactics = tactics
        self.tactic_to_idx = {t: i for i, t in enumerate(tactics)}
        self.idx_to_tactic = {i: t for i, t in enumerate(tactics)}
        
        # Load model
        self.model = TacticSuggestionModel(
            vocab_size=len(vocab),
            num_tactics=len(tactics)
        )
        self.model.load_state_dict(torch.load(model_path, map_location='cpu'))
        self.model.eval()
    
    def suggest_tactics(self, goal_type: str, context: List[Tuple[str, str]], 
                       top_k: int = 3) -> List[Tuple[str, float]]:
        """Suggest tactics for given goal and context"""
        # Convert to tensors
        goal_type_idx = self.vocab.get(goal_type, 0)
        context_indices = [self.vocab.get(var, 0) for var, _ in context]
        
        # Pad context
        max_context_len = 10
        if len(context_indices) > max_context_len:
            context_indices = context_indices[:max_context_len]
        else:
            context_indices.extend([0] * (max_context_len - len(context_indices)))
        
        goal_tensor = torch.tensor([goal_type_idx], dtype=torch.long)
        context_tensor = torch.tensor([context_indices], dtype=torch.long)
        
        # Predict
        with torch.no_grad():
            logits = self.model(goal_tensor, context_tensor)
            probabilities = torch.softmax(logits, dim=1)
            
            # Get top-k tactics
            top_probs, top_indices = torch.topk(probabilities, top_k)
            
            suggestions = []
            for prob, idx in zip(top_probs[0], top_indices[0]):
                tactic = self.idx_to_tactic[idx.item()]
                confidence = prob.item()
                suggestions.append((tactic, confidence))
            
            return suggestions


# ============================================================================
# MAIN
# ============================================================================

def main():
    print("="*60)
    print("  Neural Model for Tactic Suggestion")
    print("  PyTorch Implementation")
    print("="*60)
    
    # Check if CUDA is available
    device = 'cuda' if torch.cuda.is_available() else 'cpu'
    print(f"Using device: {device}")
    
    # Create vocabulary (simplified)
    vocab = {
        'A': 1, 'B': 2, 'C': 3, 'D': 4,
        'A -> B': 5, 'B -> C': 6, 'A -> C': 7,
        'A & B': 8, 'A | B': 9, '~A': 10,
        '<UNK>': 0
    }
    
    tactics = ['intro', 'exact', 'assumption', 'apply', 
               'split', 'left', 'right', 'contradiction']
    
    # Create model
    model = TacticSuggestionModel(
        vocab_size=len(vocab),
        embedding_dim=64,
        hidden_dim=128,
        num_tactics=len(tactics)
    )
    
    print(f"Model parameters: {sum(p.numel() for p in model.parameters()):,}")
    
    # Create dummy dataset
    print("\nCreating dummy dataset...")
    # In real implementation, load from actual data
    
    print("\nModel architecture created successfully!")
    print("Ready for training with real data.")
    
    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())
