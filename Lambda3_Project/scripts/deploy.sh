#!/bin/bash
# Lambda³ Production Deployment Script
# Deploy to cloud with monitoring

set -e

echo "=========================================="
echo "  Lambda³ Production Deployment"
echo "=========================================="

# Configuration
PROJECT_NAME="lambda3"
REGISTRY="your-registry.com"
VERSION="1.0.0"
NAMESPACE="lambda3"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check prerequisites
check_prerequisites() {
    log_info "Checking prerequisites..."
    
    # Check Docker
    if ! command -v docker &> /dev/null; then
        log_error "Docker not found. Please install Docker."
        exit 1
    fi
    
    # Check kubectl
    if ! command -v kubectl &> /dev/null; then
        log_error "kubectl not found. Please install kubectl."
        exit 1
    fi
    
    # Check helm
    if ! command -v helm &> /dev/null; then
        log_warn "Helm not found. Some features may not work."
    fi
    
    log_info "Prerequisites check passed"
}

# Build Docker image
build_image() {
    log_info "Building Docker image..."
    
    # Build image
    docker build -t ${PROJECT_NAME}:${VERSION} .
    docker tag ${PROJECT_NAME}:${VERSION} ${PROJECT_NAME}:latest
    
    # Push to registry (if configured)
    if [ ! -z "$REGISTRY" ]; then
        log_info "Pushing to registry..."
        docker tag ${PROJECT_NAME}:${VERSION} ${REGISTRY}/${PROJECT_NAME}:${VERSION}
        docker push ${REGISTRY}/${PROJECT_NAME}:${VERSION}
    fi
    
    log_info "Docker image built successfully"
}

# Deploy to Kubernetes
deploy_k8s() {
    log_info "Deploying to Kubernetes..."
    
    # Create namespace
    kubectl create namespace ${NAMESPACE} --dry-run=client -o yaml | kubectl apply -f -
    
    # Apply Kubernetes manifests
    kubectl apply -f k8s/deployment.yaml -n ${NAMESPACE}
    
    # Wait for deployment
    log_info "Waiting for deployment to be ready..."
    kubectl wait --for=condition=available --timeout=300s deployment/lambda3-api -n ${NAMESPACE}
    
    # Check status
    kubectl get pods -n ${NAMESPACE}
    kubectl get services -n ${NAMESPACE}
    
    log_info "Kubernetes deployment completed"
}

# Setup monitoring
setup_monitoring() {
    log_info "Setting up monitoring..."
    
    # Create monitoring namespace
    kubectl create namespace monitoring --dry-run=client -o yaml | kubectl apply -f -
    
    # Deploy Prometheus (if available)
    if command -v helm &> /dev/null; then
        log_info "Installing Prometheus..."
        helm repo add prometheus-community https://prometheus-community.github.io/helm-charts
        helm repo update
        helm install prometheus prometheus-community/kube-prometheus-stack \
            --namespace monitoring \
            --create-namespace \
            --set grafana.adminPassword=admin
    else
        log_warn "Helm not available. Skipping Prometheus installation."
    fi
    
    log_info "Monitoring setup completed"
}

# Run health checks
health_check() {
    log_info "Running health checks..."
    
    # Get service URL
    SERVICE_URL=$(kubectl get service lambda3-api-service -n ${NAMESPACE} -o jsonpath='{.status.loadBalancer.ingress[0].ip}')
    if [ -z "$SERVICE_URL" ]; then
        SERVICE_URL="localhost"
    fi
    
    # Test health endpoint
    log_info "Testing health endpoint..."
    curl -f http://${SERVICE_URL}/health || log_error "Health check failed"
    
    # Test API endpoints
    log_info "Testing API endpoints..."
    curl -f http://${SERVICE_URL}/version || log_error "Version endpoint failed"
    
    log_info "Health checks passed"
}

# Setup ingress
setup_ingress() {
    log_info "Setting up ingress..."
    
    # Create ingress manifest
    cat <<EOF | kubectl apply -f -
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: lambda3-ingress
  namespace: ${NAMESPACE}
  annotations:
    nginx.ingress.kubernetes.io/rewrite-target: /
spec:
  rules:
  - host: lambda3.your-domain.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: lambda3-api-service
            port:
              number: 80
EOF
    
    log_info "Ingress setup completed"
}

# Main deployment function
main() {
    echo "Starting Lambda³ deployment..."
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --skip-build)
                SKIP_BUILD=true
                shift
                ;;
            --skip-monitoring)
                SKIP_MONITORING=true
                shift
                ;;
            --registry)
                REGISTRY="$2"
                shift 2
                ;;
            --version)
                VERSION="$2"
                shift 2
                ;;
            *)
                log_error "Unknown option $1"
                exit 1
                ;;
        esac
    done
    
    # Run deployment steps
    check_prerequisites
    
    if [ "$SKIP_BUILD" != "true" ]; then
        build_image
    fi
    
    deploy_k8s
    
    if [ "$SKIP_MONITORING" != "true" ]; then
        setup_monitoring
    fi
    
    setup_ingress
    health_check
    
    log_info "Deployment completed successfully!"
    log_info "Access your Lambda³ API at: http://lambda3.your-domain.com"
    log_info "API Documentation: http://lambda3.your-domain.com/docs"
}

# Run main function
main "$@"
