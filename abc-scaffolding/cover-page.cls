\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{cover-page}

\LoadClass{article}
\RequirePackage[letterpaper, margin=1in]{geometry}
\RequirePackage{fontspec}
\RequirePackage{fancyhdr}
\RequirePackage{etoolbox}

\setmainfont{TeX Gyre Termes}

\newcommand*{\composer}[1]{\gdef\@composer{#1}}
\renewcommand*{\title}[1]{\gdef\@title{#1}}
\newcommand*{\subtitle}[1]{\gdef\@subtitle{#1}}
\newcommand*{\catalog}[2]{\gdef\@catalogname{#1}\gdef\@catalognum{#2}}

\renewcommand*{\maketitle}{
  \begin{center}
  \hbox{}
  \vfill

  \fontsize{32pt}{36pt}\selectfont
  \@title

  \ifdef{\@subtitle}{
    \bigskip
    \fontsize{20pt}{22pt}\selectfont
    \@subtitle
  }{
  }

  \vfill

  \fontsize{20pt}{22pt}\fontshape{it}\selectfont
  \@composer

  \vfill
  \end{center}
}

\pagestyle{fancy}
\lhead{}
\chead{}
\rhead{}
\lfoot{}
\cfoot{}
\rfoot{%
  \ifdef{\@catalogname}{%
    \@catalogname~\@catalognum%
  }{%
  }%
}
\renewcommand{\headrulewidth}{0pt}
\renewcommand{\footrulewidth}{0pt}
