# 🚀 Getting Started with Lambda³

Welcome to Lambda³ - the first hybrid neural-symbolic AI on ternary substrate!

---

## 📋 Prerequisites

- **Python 3.11+**
- **pip** (Python package manager)
- **Git**

---

## 🔧 Installation

### 1. Clone Repository

```bash
git clone https://github.com/lambda3-project/lambda3.git
cd lambda3
```

### 2. Create Virtual Environment

```bash
# Create venv
python -m venv venv

# Activate (Windows)
venv\Scripts\activate

# Activate (Unix/Mac)
source venv/bin/activate
```

### 3. Install Dependencies

```bash
# Install package in development mode
pip install -e .

# Or install from requirements
pip install -r requirements.txt
```

---

## ✅ Verify Installation

```bash
# Run tests
pytest tests/

# Run example
python examples/hello_lambda.py
```

**Expected output:**
```
============================================================
Lambda³ - Hello Lambda Example
============================================================

1. Identity Function
   Input:  λx.x
   Parsed: (λx.x)
   Trits:  [1, 0]

2. Application
   Input:  (λx.x) y
   Parsed: ((λx.x) y)
   Result: y

3. Why Ternary is Perfect for Lambda
   Lambda has 3 constructs:
   - Variable    → 0 (trit)
   - Abstraction → 1 (trit)
   - Application → 2 (trit)
   
   ...
```

---

## 📚 Quick Tutorial

### Parse a Lambda Term

```python
from lambda3 import parse

# Parse identity function
term = parse("λx.x")
print(term)  # (λx.x)
```

### Reduce a Lambda Term

```python
from lambda3 import parse, reduce

# Parse and reduce
term = parse("(λx.x) y")
result = reduce(term)
print(result)  # y
```

### Ternary Encoding

```python
from lambda3 import parse, encode

# Encode in ternary
term = parse("λx.x")
trits = encode(term)
print([t.value for t in trits])  # [1, 0]
# 1 = Abstraction, 0 = Variable
```

---

## 🎯 Next Steps

### For Users:
1. Read [README.md](README.md) for overview
2. Try [examples/](examples/) for more examples
3. Read [docs/](docs/) for detailed docs

### For Developers:
1. Read [list_todo5.md](../list_todo5.md) for roadmap
2. Check [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines
3. Join [Discord](https://discord.gg/lambda3) for discussions

### For Researchers:
1. Read [papers/](papers/) for research
2. Read file "plus" for philosophical vision
3. Read [VISIONE_LAMBDA_TERNARY.md](../TEROS_Project/VISIONE_LAMBDA_TERNARY.md)

---

## 🔗 Current Status

**Phase 1: MVP Lambda Engine**
- [x] Project structure ✅
- [x] Basic parser (placeholder)
- [x] Basic reducer (placeholder)
- [x] Basic encoder (placeholder)
- [ ] Full parser (TODO - see list_todo5.md)
- [ ] Full reducer (TODO)
- [ ] Full encoder (TODO)
- [ ] REPL (TODO)

**Progress: 15%** | **Target: Month 2**

---

## 🆘 Troubleshooting

### Import Error

**Problem:** `ModuleNotFoundError: No module named 'lambda3'`

**Solution:**
```bash
# Make sure you installed in development mode
pip install -e .

# Or add to PYTHONPATH
export PYTHONPATH="${PYTHONPATH}:$(pwd)"
```

### Test Failures

**Problem:** Tests fail

**Solution:**
```bash
# Make sure dependencies are installed
pip install -r requirements.txt

# Run tests with verbose output
pytest -v tests/
```

---

## 📞 Getting Help

- **Documentation:** [docs/](docs/)
- **Issues:** [GitHub Issues](https://github.com/lambda3-project/lambda3/issues)
- **Discord:** [Join Server](https://discord.gg/lambda3)
- **Email:** team@lambda3.ai

---

## 🎓 Learning Resources

### Lambda Calculus:
- [Wikipedia](https://en.wikipedia.org/wiki/Lambda_calculus)
- Book: "The Lambda Calculus" by Barendregt
- Course: "Functional Programming" (Coursera)

### Type Theory:
- Book: "Types and Programming Languages" by Pierce
- Course: "Programming Languages" (Coursera)

### Neural-Symbolic AI:
- Paper: "Neural-Symbolic Integration" (various)
- Book: "Neural-Symbolic Cognitive Reasoning"

---

## 💡 Philosophy

Remember: You're not just learning a library.

**You're learning a new way to think about AI.**

> "Current AI approximates. Lambda³ AI reasons."

---

**Happy coding!** 🚀

*"The future of AI is Lambda + Ternary + Neural."*

