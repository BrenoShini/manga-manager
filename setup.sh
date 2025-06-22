#!/bin/bash

# Script de configuração para o Sistema de Gerenciamento de Mangás
# Ubuntu/Debian setup script

echo "Configurando o Sistema de Gerenciamento de Mangás..."
echo ""

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para imprimir com cores
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Verificar se está no Ubuntu/Debian
if ! command -v apt &> /dev/null; then
    print_error "Este script é para sistemas baseados em Debian/Ubuntu"
    exit 1
fi

print_status "Verificando dependências..."

# Verificar se o GCC está instalado
if ! command -v gcc &> /dev/null; then
    print_warning "GCC não encontrado. Instalando..."
    sudo apt update
    sudo apt install -y build-essential
    
    if command -v gcc &> /dev/null; then
        print_success "GCC instalado com sucesso!"
    else
        print_error "Falha ao instalar GCC"
        exit 1
    fi
else
    print_success "GCC já está instalado ($(gcc --version | head -n1))"
fi

# Verificar se o Make está instalado
if ! command -v make &> /dev/null; then
    print_warning "Make não encontrado. Instalando..."
    sudo apt install -y make
    
    if command -v make &> /dev/null; then
        print_success "Make instalado com sucesso!"
    else
        print_error "Falha ao instalar Make"
        exit 1
    fi
else
    print_success "Make já está instalado ($(make --version | head -n1))"
fi

# Verificar se o Git está instalado (para clonar o repo)
if ! command -v git &> /dev/null; then
    print_warning "Git não encontrado. Instalando..."
    sudo apt install -y git
    
    if command -v git &> /dev/null; then
        print_success "Git instalado com sucesso!"
    else
        print_error "Falha ao instalar Git"
        exit 1
    fi
else
    print_success "Git já está instalado ($(git --version))"
fi

print_status "Compilando o projeto..."

# Compilar o projeto
if make; then
    print_success "Projeto compilado com sucesso!"
else
    print_error "Falha na compilação"
    exit 1
fi

print_status "Verificando arquivo de dados iniciais..."

# Verificar se o arquivo de dados existe
if [ ! -f "mangas.txt" ]; then
    print_warning "Arquivo mangas.txt não encontrado!"
    print_status "Criando arquivo de exemplo..."
    
    cat > mangas.txt << 'EOF'
978-4-08-883267-8; Frieren: Beyond Journey's End; Kanehito Yamada, Tsukasa Abe; 2020; -; Fantasia, Drama; Weekly Shōnen Sunday; Shogakukan; 2022; 12; 5; [1, 2, 3, 4, 5]
978-4-08-871003-7; Yu Yu Hakusho; Yoshihiro Togashi; 1990; 1994; Ação, Artes Marciais, Sobrenatural; Weekly Shōnen Jump; Shueisha; 2004; 19; 19; [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
978-4-08-871610-7; Hunter x Hunter; Yoshihiro Togashi; 1998; -; Ação; Weekly Shōnen Jump; Shueisha; 2016; 37; 10; [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
978-4-08-882173-3; Spy × Family; Tatsuya Endo; 2019; -; Comédia; Shōnen Jump+; Shueisha; 2020; 12; 7; [1, 2, 3, 4, 5, 6, 7]
978-4-08-880723-2; Demon Slayer; Koyoharu Gotouge; 2016; 2020; Ação; Weekly Shōnen Jump; Shueisha; 2018; 23; 23; [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23]
978-4-04-713151-0; Neon Genesis Evangelion; Yoshiyuki Sadamoto; 1994; 2013; Mecha, Drama; Shōnen Ace; Kadokawa Shoten; 2005; 14; 8; [1, 2, 3, 4, 5, 6, 7, 8]
978-4-08-881780-4; Chainsaw Man; Tatsuki Fujimoto; 2018; 2020; Ação; Weekly Shōnen Jump; Shueisha; 2019; 11; 6; [1, 2, 3, 4, 5, 6]
978-4-08-873113-1; Naruto; Masashi Kishimoto; 1999; 2014; Ação; Weekly Shōnen Jump; Shueisha; 2008; 72; 25; [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
EOF
    
    print_success "Arquivo mangas.txt criado com dados de exemplo"
else
    print_success "Arquivo mangas.txt encontrado"
fi

echo ""
echo "🎉 Configuração concluída com sucesso!"
echo ""
print_status "Para executar o programa:"
echo "   make run"
echo ""
print_status "Comandos disponíveis:"
echo "   make        - Compilar o programa"
echo "   make run    - Executar o programa" 
echo "   make clean  - Limpar arquivos compilados e dados"
echo ""
print_warning "Dica: Na primeira execução, escolha a opção 6 para carregar os dados iniciais!"
echo ""

# Perguntar se quer executar agora
read -p "Deseja executar o programa agora? (s/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Ss]$ ]]; then
    print_status "Iniciando o programa..."
    echo ""
    ./manga_manager
fi

print_success "Setup concluído! Divirta-se!"
