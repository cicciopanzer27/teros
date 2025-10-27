"""
Neural Training Pipeline for Lambda³
Train PyTorch model on proof dataset
"""

import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, random_split
import json
import os
from pathlib import Path
from typing import Dict, List, Tuple
import matplotlib.pyplot as plt

try:
    from lambda3.neural.model import TacticSuggestionModel, TacticDataset, TacticTrainer
    from lambda3.neural.dataset import DatasetGenerator
except ImportError:
    import sys
    sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    from neural.model import TacticSuggestionModel, TacticDataset, TacticTrainer
    from neural.dataset import DatasetGenerator


class NeuralTrainingPipeline:
    """
    Complete training pipeline for tactic suggestion
    """
    
    def __init__(self, data_dir: str = "data", model_dir: str = "models"):
        self.data_dir = Path(data_dir)
        self.model_dir = Path(model_dir)
        self.data_dir.mkdir(exist_ok=True)
        self.model_dir.mkdir(exist_ok=True)
        
        # Create vocabulary
        self.vocab = self._create_vocabulary()
        self.tactics = ['intro', 'exact', 'assumption', 'apply', 
                       'split', 'left', 'right', 'contradiction']
        
        print(f"Vocabulary size: {len(self.vocab)}")
        print(f"Tactics: {self.tactics}")
    
    def _create_vocabulary(self) -> Dict[str, int]:
        """Create vocabulary from proof corpus"""
        vocab = {
            '<PAD>': 0,
            '<UNK>': 1,
            'A': 2, 'B': 3, 'C': 4, 'D': 5,
            'A -> B': 6, 'B -> C': 7, 'A -> C': 8,
            'A & B': 9, 'A | B': 10, '~A': 11,
            'P': 12, 'Q': 13, 'R': 14, 'S': 15,
            'x': 16, 'y': 17, 'z': 18,
            'f': 19, 'g': 20, 'h': 21,
        }
        return vocab
    
    def prepare_dataset(self, num_proofs: int = 2000):
        """Generate and prepare training dataset"""
        print(f"\nGenerating {num_proofs} proof examples...")
        
        # Generate dataset
        generator = DatasetGenerator()
        generator.generate_synthetic_proofs(count=num_proofs)
        
        # Export to JSON
        data_file = self.data_dir / "training_proofs.json"
        generator.export_to_json(str(data_file))
        
        print(f"Dataset saved to {data_file}")
        return str(data_file)
    
    def create_model(self, embedding_dim: int = 128, hidden_dim: int = 256):
        """Create neural model"""
        model = TacticSuggestionModel(
            vocab_size=len(self.vocab),
            embedding_dim=embedding_dim,
            hidden_dim=hidden_dim,
            num_tactics=len(self.tactics)
        )
        
        param_count = sum(p.numel() for p in model.parameters())
        print(f"Model created: {param_count:,} parameters")
        
        return model
    
    def create_dataloaders(self, data_file: str, batch_size: int = 32, 
                          train_split: float = 0.8):
        """Create training and validation dataloaders"""
        
        # Load dataset
        dataset = TacticDataset(data_file, self.vocab, self.tactics)
        
        # Split dataset
        train_size = int(len(dataset) * train_split)
        val_size = len(dataset) - train_size
        train_dataset, val_dataset = random_split(dataset, [train_size, val_size])
        
        # Create dataloaders
        train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
        val_loader = DataLoader(val_dataset, batch_size=batch_size, shuffle=False)
        
        print(f"Dataset split: {train_size} train, {val_size} validation")
        
        return train_loader, val_loader
    
    def train_model(self, model: TacticSuggestionModel, train_loader: DataLoader,
                   val_loader: DataLoader, epochs: int = 50, device: str = 'cpu'):
        """Train the neural model"""
        
        print(f"\nStarting training on {device}...")
        print(f"Epochs: {epochs}")
        print(f"Train batches: {len(train_loader)}")
        print(f"Val batches: {len(val_loader)}")
        
        # Create trainer
        trainer = TacticTrainer(model, device)
        
        # Training loop
        history = trainer.train(train_loader, val_loader, epochs)
        
        # Save final model
        model_path = self.model_dir / "final_tactic_model.pth"
        torch.save(model.state_dict(), model_path)
        print(f"Model saved to {model_path}")
        
        return history
    
    def plot_training_history(self, history: Dict, save_path: str = None):
        """Plot training history"""
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))
        
        # Loss plot
        ax1.plot(history['train_loss'], label='Train Loss')
        ax1.plot(history['val_loss'], label='Validation Loss')
        ax1.set_xlabel('Epoch')
        ax1.set_ylabel('Loss')
        ax1.set_title('Training Loss')
        ax1.legend()
        ax1.grid(True)
        
        # Accuracy plot
        ax2.plot(history['train_acc'], label='Train Accuracy')
        ax2.plot(history['val_acc'], label='Validation Accuracy')
        ax2.set_xlabel('Epoch')
        ax2.set_ylabel('Accuracy')
        ax2.set_title('Training Accuracy')
        ax2.legend()
        ax2.grid(True)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path)
            print(f"Training plots saved to {save_path}")
        
        plt.show()
    
    def evaluate_model(self, model: TacticSuggestionModel, val_loader: DataLoader,
                      device: str = 'cpu'):
        """Evaluate trained model"""
        trainer = TacticTrainer(model, device)
        val_loss, val_acc = trainer.evaluate(val_loader)
        
        print(f"\nFinal Evaluation:")
        print(f"   Validation Loss: {val_loss:.4f}")
        print(f"   Validation Accuracy: {val_acc:.4f}")
        
        return val_loss, val_acc
    
    def run_full_training(self, num_proofs: int = 2000, epochs: int = 50):
        """Run complete training pipeline"""
        print("="*60)
        print("  Lambda³ Neural Training Pipeline")
        print("  Training Tactic Suggestion Model")
        print("="*60)
        
        # Check device
        device = 'cuda' if torch.cuda.is_available() else 'cpu'
        print(f"Using device: {device}")
        
        # 1. Prepare dataset
        data_file = self.prepare_dataset(num_proofs)
        
        # 2. Create model
        model = self.create_model()
        
        # 3. Create dataloaders
        train_loader, val_loader = self.create_dataloaders(data_file)
        
        # 4. Train model
        history = self.train_model(model, train_loader, val_loader, epochs, device)
        
        # 5. Evaluate
        self.evaluate_model(model, val_loader, device)
        
        # 6. Plot results
        plot_path = self.model_dir / "training_history.png"
        self.plot_training_history(history, str(plot_path))
        
        print("\n" + "="*60)
        print("Training Complete!")
        print("="*60)
        
        return model, history


def main():
    """Run training pipeline"""
    pipeline = NeuralTrainingPipeline()
    
    # Run full training
    model, history = pipeline.run_full_training(
        num_proofs=1000,  # Smaller for demo
        epochs=20         # Fewer epochs for demo
    )
    
    print("\nNext Steps:")
    print("  1. Use trained model for tactic suggestion")
    print("  2. Integrate with proof assistant")
    print("  3. Collect more real proof data")
    print("  4. Fine-tune on domain-specific proofs")
    
    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())
