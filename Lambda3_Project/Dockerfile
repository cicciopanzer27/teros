# Lambda³ Docker Container
# Production-ready deployment

FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

# Copy requirements
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application code
COPY lambda3/ ./lambda3/
COPY examples/ ./examples/
COPY benchmarks/ ./benchmarks/
COPY tests/ ./tests/
COPY setup.py .
COPY README.md .

# Install Lambda³ package
RUN pip install -e .

# Expose port for REST API
EXPOSE 8000

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD python -c "from lambda3.parser.parser import parse; parse('\\x.x')" || exit 1

# Default command: run REST API server
CMD ["python", "-m", "lambda3.api.rest_api"]

