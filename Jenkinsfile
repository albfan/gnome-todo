def base_image = "waltervargas/gnome-todo"

node {
  checkout scm

  stage("Building"){
    sh "rm -rf /var/lib/jenkins/builds/*"
    sh "PATH=$PATH:/usr/local/bin flatpak-builder --force-clean --repo=repo dist org.gnome.Todo.Test.json"
  }

  stage("Testing"){
    sh "PATH=$PATH:/usr/local/bin flatpak-builder --run dist org.gnome.Todo.Test.json gnome-desktop-testing-runner gnome-todo > test-suite.log"
    step([$class: "TapPublisher", testResults: "test-suite.log"])
  }
}
