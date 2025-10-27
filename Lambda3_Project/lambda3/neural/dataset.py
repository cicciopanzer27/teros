"""
Neural Dataset Creation for Tactic Suggestion
Prepare training data from proof corpus
"""

import json
from typing import List, Dict, Tuple
from dataclasses import dataclass, asdict
from pathlib import Path


@dataclass
class ProofStep:
    """Single step in a proof"""
    goal_type: str
    context: List[Tuple[str, str]]  # [(var, type)]
    tactic_used: str
    success: bool
    proof_id: str
    step_number: int


@dataclass
class ProofExample:
    """Complete proof example"""
    proof_id: str
    theorem_name: str
    goal_type: str
    steps: List[ProofStep]
    final_success: bool


class DatasetGenerator:
    """
    Generate training dataset from proof corpus
    
    Target: 10,000+ proof examples
    """
    
    def __init__(self):
        self.proofs: List[ProofExample] = []
        self.tactics = [
            'intro', 'exact', 'assumption', 'apply',
            'split', 'left', 'right', 'contradiction'
        ]
    
    def add_proof(self, proof: ProofExample):
        """Add a proof to the dataset"""
        self.proofs.append(proof)
    
    def generate_synthetic_proofs(self, count: int = 1000):
        """
        Generate synthetic proof examples
        
        For real training, these would come from:
        - Human-written proofs
        - Coq/Lean proof libraries
        - Mathematical proof databases
        """
        print(f"Generating {count} synthetic proofs...")
        
        # Simple proof patterns
        patterns = [
            self._generate_identity_proof,
            self._generate_modus_ponens_proof,
            self._generate_transitivity_proof,
            self._generate_conjunction_proof,
        ]
        
        for i in range(count):
            pattern = patterns[i % len(patterns)]
            proof = pattern(f"synthetic_{i}")
            self.add_proof(proof)
        
        print(f"Generated {len(self.proofs)} proofs")
    
    def _generate_identity_proof(self, proof_id: str) -> ProofExample:
        """Generate proof of A -> A"""
        steps = [
            ProofStep(
                goal_type="A -> A",
                context=[],
                tactic_used="intro",
                success=True,
                proof_id=proof_id,
                step_number=0
            ),
            ProofStep(
                goal_type="A",
                context=[("x", "A")],
                tactic_used="assumption",
                success=True,
                proof_id=proof_id,
                step_number=1
            )
        ]
        
        return ProofExample(
            proof_id=proof_id,
            theorem_name="identity",
            goal_type="A -> A",
            steps=steps,
            final_success=True
        )
    
    def _generate_modus_ponens_proof(self, proof_id: str) -> ProofExample:
        """Generate proof of (A -> B) -> A -> B"""
        steps = [
            ProofStep(
                goal_type="(A -> B) -> A -> B",
                context=[],
                tactic_used="intro",
                success=True,
                proof_id=proof_id,
                step_number=0
            ),
            ProofStep(
                goal_type="A -> B",
                context=[("f", "A -> B")],
                tactic_used="intro",
                success=True,
                proof_id=proof_id,
                step_number=1
            ),
            ProofStep(
                goal_type="B",
                context=[("f", "A -> B"), ("x", "A")],
                tactic_used="apply",
                success=True,
                proof_id=proof_id,
                step_number=2
            )
        ]
        
        return ProofExample(
            proof_id=proof_id,
            theorem_name="modus_ponens",
            goal_type="(A -> B) -> A -> B",
            steps=steps,
            final_success=True
        )
    
    def _generate_transitivity_proof(self, proof_id: str) -> ProofExample:
        """Generate proof of transitivity"""
        steps = [
            ProofStep(
                goal_type="(A -> B) -> (B -> C) -> (A -> C)",
                context=[],
                tactic_used="intro",
                success=True,
                proof_id=proof_id,
                step_number=0
            ),
            ProofStep(
                goal_type="(B -> C) -> (A -> C)",
                context=[("f", "A -> B")],
                tactic_used="intro",
                success=True,
                proof_id=proof_id,
                step_number=1
            ),
            ProofStep(
                goal_type="A -> C",
                context=[("f", "A -> B"), ("g", "B -> C")],
                tactic_used="intro",
                success=True,
                proof_id=proof_id,
                step_number=2
            ),
            ProofStep(
                goal_type="C",
                context=[("f", "A -> B"), ("g", "B -> C"), ("x", "A")],
                tactic_used="apply",
                success=True,
                proof_id=proof_id,
                step_number=3
            )
        ]
        
        return ProofExample(
            proof_id=proof_id,
            theorem_name="transitivity",
            goal_type="(A -> B) -> (B -> C) -> (A -> C)",
            steps=steps,
            final_success=True
        )
    
    def _generate_conjunction_proof(self, proof_id: str) -> ProofExample:
        """Generate proof involving conjunction"""
        steps = [
            ProofStep(
                goal_type="A -> B -> (A & B)",
                context=[],
                tactic_used="intro",
                success=True,
                proof_id=proof_id,
                step_number=0
            ),
            ProofStep(
                goal_type="B -> (A & B)",
                context=[("a", "A")],
                tactic_used="intro",
                success=True,
                proof_id=proof_id,
                step_number=1
            ),
            ProofStep(
                goal_type="A & B",
                context=[("a", "A"), ("b", "B")],
                tactic_used="split",
                success=True,
                proof_id=proof_id,
                step_number=2
            )
        ]
        
        return ProofExample(
            proof_id=proof_id,
            theorem_name="conjunction_intro",
            goal_type="A -> B -> (A & B)",
            steps=steps,
            final_success=True
        )
    
    def export_to_json(self, filepath: str):
        """Export dataset to JSON"""
        data = {
            'version': '1.0',
            'count': len(self.proofs),
            'tactics': self.tactics,
            'proofs': [asdict(proof) for proof in self.proofs]
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Exported {len(self.proofs)} proofs to {filepath}")
    
    def get_training_data(self) -> List[Tuple[Dict, str]]:
        """
        Get data in format for training
        
        Returns:
            List of (features, label) pairs
            features: {goal_type, context}
            label: tactic_used
        """
        training_data = []
        
        for proof in self.proofs:
            for step in proof.steps:
                features = {
                    'goal_type': step.goal_type,
                    'context': step.context,
                    'proof_id': step.proof_id
                }
                label = step.tactic_used
                
                training_data.append((features, label))
        
        return training_data
    
    def print_statistics(self):
        """Print dataset statistics"""
        print("\n" + "="*60)
        print("Dataset Statistics")
        print("="*60)
        print(f"Total proofs: {len(self.proofs)}")
        
        total_steps = sum(len(proof.steps) for proof in self.proofs)
        print(f"Total proof steps: {total_steps}")
        
        success_count = sum(1 for proof in self.proofs if proof.final_success)
        print(f"Successful proofs: {success_count}/{len(self.proofs)}")
        
        # Tactic distribution
        tactic_counts = {}
        for proof in self.proofs:
            for step in proof.steps:
                tactic = step.tactic_used
                tactic_counts[tactic] = tactic_counts.get(tactic, 0) + 1
        
        print("\nTactic distribution:")
        for tactic, count in sorted(tactic_counts.items(), key=lambda x: x[1], reverse=True):
            percentage = count / total_steps * 100
            print(f"  {tactic:15s}: {count:5d} ({percentage:5.1f}%)")
        
        print("="*60)


def main():
    print("="*60)
    print("  Neural Dataset Generation")
    print("  Proof Corpus Preparation")
    print("="*60)
    
    # Generate dataset
    dataset = DatasetGenerator()
    dataset.generate_synthetic_proofs(count=1000)
    
    # Print statistics
    dataset.print_statistics()
    
    # Export
    output_path = "data/training_proofs.json"
    Path("data").mkdir(exist_ok=True)
    dataset.export_to_json(output_path)
    
    # Get training data
    training_data = dataset.get_training_data()
    print(f"\nTraining examples: {len(training_data)}")
    
    print("\n" + "="*60)
    print("Dataset Generation Complete")
    print("="*60)
    
    print("\n🎯 Next Steps:")
    print("  1. Train neural model on this data")
    print("  2. Collect real proofs from Coq/Lean")
    print("  3. Augment with human-written proofs")
    print("  4. Fine-tune model")
    
    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())

