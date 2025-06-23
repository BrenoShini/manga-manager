# Sistema de Gerenciamento de Mangás

Sistema desenvolvido para a disciplina **Algoritmos e Estruturas de Dados II** da USP.

Um programa em C que implementa um sistema CRUD completo para gerenciamento de coleções de mangás, utilizando índices primários e secundários para otimização de buscas.

## Funcionalidades

- **CRUD Completo**: Criar, ler, atualizar e deletar registros de mangás
- **Índices Primários**: Busca eficiente por ISBN (chave primária)
- **Índices Secundários**: Busca por título (fracamente ligado via ISBN)
- **Busca Binária**: Algoritmo O(log n) para consultas rápidas
- **Persistência**: Dados e índices salvos em arquivos
- **Confirmação de Deleção**: Segurança contra exclusões acidentais
- **Carregamento Inicial**: Importação de dados do arquivo texto fornecido

## Estrutura de Dados

Cada mangá contém as seguintes informações:
- **ISBN**: Identificador único
- **Título**: Nome do mangá
- **Autor(es)**: Criador(es) da obra
- **Ano de início/fim**: Período de publicação
- **Gênero**: Categoria da obra
- **Revista**: Onde foi publicado
- **Editora**: Responsável pela edição
- **Ano da edição**: Versão específica
- **Volumes**: Total e lista dos volumes adquiridos

## Como Executar

## Pré-requisitos
- GCC (GNU Compiler Collection)
- Make
- Sistema Linux/Unix (testado no Ubuntu)

### Instalação
```bash
# Clone o repositório
git clone https://github.com/BrenoShini/manga-manager.git
cd manga-manager

# Compile o programa
make

# Execute
make run
```

### Primeira Execução
1. Execute o programa
2. Escolha a opção **6** para carregar os dados iniciais
3. Explore as funcionalidades do menu

## Menu Principal

```
=== SISTEMA DE GERENCIAMENTO DE MANGÁS ===
1. Criar mangá
2. Buscar mangá
3. Atualizar mangá
4. Deletar mangá
5. Listar todos os mangás
6. Carregar dados iniciais
0. Sair
```

## Arquitetura do Sistema

### Índices Implementados

**Índice Primário (ISBN)**
- Acesso direto aos registros
- Ordenado para busca binária
- Armazenado em `primary_index.dat`

**Índice Secundário (Título)**
- Fracamente ligado via ISBN
- Permite busca por nome do mangá
- Armazenado em `secondary_index.dat`

### Arquivos do Sistema
```
manga-manager/
├── manga_manager.c      # Código fonte principal
├── Makefile            # Arquivo de compilação
├── mangas.txt          # Dados iniciais fornecidos
├── mangas.dat          # Arquivo de dados binário (criado automaticamente)
├── primary_index.dat   # Índices primários (criado automaticamente)
├── secondary_index.dat # Índices secundários (criado automaticamente)
└── README.md          # Este arquivo
```

## Exemplos de Uso

### Buscar um Mangá
```
=== BUSCAR MANGÁ ===
Digite o ISBN ou título: Naruto
```

### Criar Novo Mangá
```
=== CRIAR NOVO MANGÁ ===
ISBN: 978-1-23-456789-0
Título: Attack on Titan
Autor(es): Hajime Isayama
...
```

### Atualizar Coleção
```
=== ATUALIZAR MANGÁ ===
Digite o ISBN ou título do mangá a ser atualizado: Attack on Titan
Novos volumes adquiridos (-1 para manter): 10
```

## Dados de Teste

O sistema inclui dados iniciais de mangás populares:
- Frieren: Beyond Journey's End
- Yu Yu Hakusho
- Hunter x Hunter
- Spy × Family
- Demon Slayer
- Neon Genesis Evangelion
- Chainsaw Man
- Naruto

## Comandos Úteis

```bash
# Compilar
make

# Executar
make run

# Limpar arquivos compilados e dados
make clean

# Recompilar tudo
make clean && make
```

## Conceitos Técnicos Aplicados

- **Estruturas de Dados**: Arrays dinâmicos, structs
- **Algoritmos de Ordenação**: qsort() para manutenção de índices
- **Algoritmos de Busca**: Busca binária para eficiência O(log n)
- **Gerenciamento de Arquivos**: I/O binário para persistência
- **Índices de Banco de Dados**: Primários e secundários
- **CRUD Operations**: Create, Read, Update, Delete

## 🎥 Demonstração

Um vídeo de demonstração do sistema em funcionamento está disponível [aqui](https://youtu.be/65GHQVnHDtY)

---
