def base_image = "waltervargas/gnome-todo"

node {
  checkout scm

  stage("Building"){
    sh "PATH=$PATH:/usr/local/bin flatpak-builder --force-clean --repo=repo dist org.gnome.Todo.Test.json"
  }

  stage("Testing"){
    sh "PATH=$PATH:/usr/local/bin flatpak-builder --run dist org.gnome.Todo.Test.json gnome-desktop-testing-runner gnome-todo"
  }
}
